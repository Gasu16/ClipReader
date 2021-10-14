#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <X11/Xlib.h>

Bool PrintSelection(Display *display, Window window, const char *bufname, const char *fmtname){
    char *result;
    unsigned long ressize, restail;
    int resbits;


    Atom bufid = XInternAtom(display, bufname, False), // buffer id {CLIPBOARD, PRIMARY}
         fmtid = XInternAtom(display, fmtname, False), // Format id {STRING, UTF8_STRING}
         propid = XInternAtom(display, "XSEL_DATA", False),
         incrid = XInternAtom(display, "INCR", False);

    XEvent event;

    if(XConvertSelection(display, bufid, fmtid, propid, window, CurrentTime) == None){
        perror("Error on XConvertSelection() \n");
        exit(EXIT_FAILURE);
    }

    do {
        XNextEvent(display, &event);
    } while (event.type != SelectionNotify || event.xselection.selection != bufid);

    if (event.xselection.property){
        XGetWindowProperty(display, window, propid, 0, LONG_MAX/4, False, AnyPropertyType,
        &fmtid, &resbits, &ressize, &restail, (unsigned char**)&result);

        if (fmtid == incrid)printf("Buffer is too large and INCR reading is not implemented yet.\n");
        else printf("%.*s\n", (int)ressize, result);

        XFree(result);
        return True;
    }
    else return False; // request failed, e.g. owner can't convert to the target format
}

int main(){

    Display *display;

    if((display = XOpenDisplay(NULL)) == NULL){
        perror("XOpenDisplay error: \n");
        exit(EXIT_FAILURE);
    }

    unsigned long color;

    if(color = BlackPixel(display, DefaultScreen(display))){
        perror("BlackPixel error: \n");
        exit(EXIT_FAILURE);
    }

    Window window;

    if(!(window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0,0, 1,1, 0, color, color))){
        perror("XCreatesSimpleWindow error: \n");
        exit(EXIT_FAILURE);
    }

    Bool result;

    if((result = PrintSelection(display, window, "CLIPBOARD", "UTF8_STRING")
        && PrintSelection(display, window, "PRIMARY", "UTF8_STRING")) == False){
        perror("PrintSelection error: \n");
        exit(EXIT_FAILURE);
    }

    XDestroyWindow(display, window); // Destroy the window
    XCloseDisplay(display); // Close connection to the X server
    return !result;
}
