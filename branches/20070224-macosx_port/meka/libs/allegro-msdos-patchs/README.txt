Patch for MS-DOS Allegro

- graphics_remove_auto_sync.diff
Originally from Raine/Richard Bush.
May have been tweaked.
Disable forced screen synchronization when doing various video operation.

- irq_save_fpu_state.diff
Add code to preserve FPU registers state when being interrupted.
This is not efficient, but necessary because the sound code is called
from interrupt and use floating point.
The correct solution would be not to use interrupts for this.
