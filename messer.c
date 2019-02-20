#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <xcb/xcb.h>
/*   
Command to start up zephyr for testing
Xephyr -br -ac -screen 800x600 -reset -terminate 2> /dev/null :2 & DISPLAY=:2 xterm &
*/
xcb_connection_t *connection;
xcb_screen_t *screen;
bool exitFlag = true;
xcb_generic_event_t *e;
xcb_window_t window;
uint32_t maskWindow;
uint32_t mask[1] = { XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
					 XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT};
uint32_t values[2];
/* uint32_t mask[1] = { */
/*         XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | // destroy notify */
/*         XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT }; // map request */
/* xcb_change_window_attributes(conn, screen->root, XCB_CW_EVENT_MASK, mask); */

void handleKeypress(xcb_key_press_event_t *event) {
  printf("Keyboard event."); }

int main(int argc, char *argv[]) {
  connection = xcb_connect(NULL, NULL);
  // Get the current screen
  screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
  xcb_change_window_attributes(connection, screen->root, XCB_CW_EVENT_MASK, mask);
  window = xcb_generate_id(connection);
  maskWindow = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  values[0] = screen->white_pixel;
  values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
              XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
              XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
              XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE;

  xcb_create_window(connection, 0, window, screen->root, 400, 400, 100, 100, 10,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, maskWindow,
                    values);
  xcb_map_window(connection, window);
  xcb_flush(connection);
  // Main loop
  // Either catch an event or check if the program must shutdown
  while ((e = xcb_wait_for_event(connection)) && exitFlag) {
    switch (e->response_type) {
    case XCB_ENTER_NOTIFY:
      printf("Have to enter notify you\n");
      break;
	case XCB_CREATE_NOTIFY:
	  // First event
	  printf("Create notify\n");
	  break;
	case XCB_MAP_NOTIFY:
	  // Second event
	  printf("Map notify\n");
	  break;
	case XCB_MAPPING_NOTIFY:
	  printf("Mapping notify\n");
	  break;
	case XCB_CONFIGURE_NOTIFY:
	  printf("Configure request\n");
	  break;
    case XCB_MAP_REQUEST:
      printf("Map request caught\n");
    case XCB_EXPOSE:
      /* xcb_expose_event_t *expEv = (xcb_expose_event_t *)e; */
	  // Third event
      printf("Caught an expose event!\n");
      break;
    case XCB_KEY_PRESS: {
      xcb_key_press_event_t *keyEv = (xcb_key_press_event_t *)e;
      handleKeypress(keyEv);
      break;
    }

    default:
      printf("not handled\n");
    }
    free(e);
  }
  return 0;
}
