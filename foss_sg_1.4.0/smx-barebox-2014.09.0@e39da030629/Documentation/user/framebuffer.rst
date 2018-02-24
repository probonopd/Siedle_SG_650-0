Framebuffer support
===================

barebox has support for framebuffer devices. Currently there is no console support
for framebuffers, so framebuffer usage is limited to splash screens only. barebox
supports BMP and PNG graphics using the :ref:`command_splash` command. barebox
currently has no support for backlights, so unless there is a board specific enable
hook for enabling a display it must be done manually with a script. Since barebox
has nothing useful to show on the framebuffer it doesn't enable it during startup.
A framebuffer can be enabled with the ``enable`` parameter of the framebuffer device:

.. code-block:: sh

  fb0.enable=1

Some framebuffer devices support different resolutions. These can be configured
with the ``mode_name`` parameter. See a list of supported modes using ``devinfo fb0``.
A mode can only be changed when the framebuffer is disabled.

A typical script to enable the framebuffer could look like this:

.. code-block:: sh

  #!/bin/sh

  SPLASH=/path/to/mysplash.png

  if [ ! -f $SPLASH ]; then
      exit 0
  fi

  # first show splash
  splash /path/to/mysplash.png

  # enable framebuffer
  fb0.enable=1

  # wait for signals to become stable
  msleep 100

  # finally enable backlight
  gpio_direction_output 42 1

