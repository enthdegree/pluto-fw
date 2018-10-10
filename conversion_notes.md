Changes made to pluto-fw code to integrate RPN app
-----

pluto-fw is event based, so when a button is pressed app code is called with the event passed in.

The main loop of rpn is structured to process a stream of button inputs.

- Added rpn app to `app/`
    - Added `rpn/` folder to `app/` containing: 
        - `app.c`, the app's main loop (nb the main loop contains no processing other than for the display, all the calculator things happen via calls to `svc/`)
        - `sources.mk` with line `SRC += ../common/app/rpn/app.c` 
    - Added reference to `app_t` struct for rpn in `app/apps.h`: `extern const app_t app_app_rpn;`
    - Added rpn item to launcher in `app/launcher/app.c`

- Added rpn service to `svc/`
    - Added `rpn/` folder to `svc/` containing:
        - rpn calculator internal processing 
        - `sources.mk` with `SRC += (.c files to compile in this folder)`
    - Added `rpn.h` to `svc/` containing:
	- `typedef struct svc_rpn_t {...};`, a struct definition containing all the data structures the main loop will modify
	- `svc_rpn`, a static instance of `struct svc_rpn_t`
	- Headers that wrap all the functions in `rpn/` that the main loop calls
    - Added reference to `svc/rpn.h` in `svc/svc.h`
    - Added reference to `svc/rpn.c` in `svc/sources.mk`

- Updated main loop in `/app/rpn/app.c` to call wrapped functions from `/svc/rpn.h`.
