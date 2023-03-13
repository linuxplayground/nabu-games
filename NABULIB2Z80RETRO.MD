# Porting NABU-LIB to the Retro

The NABU-LIB C Library is designed for the Nabu not the Retro.

With this statement declared early on, lets have a look at what is possible to port and what simply won't work on the Retro.

## Hardware Incompatibilities

|Hardware   |Status	      | Comments
|-----------|-----------------|---------------------------------
|PSG        | NOT COMPATIBLE  | The Retro has no PSG.
|VDP	    | COMPATIBLE      | Interrupts might need some work.
|Joystick   | NOT COMPATIBLE  | The NABU Joysticks are driven by a driver IC on the keyboard and present as ASCII on the keyboard IO Port.
|Keyboard   | NOT COMPATIBLE  | The Retro does not have a keyboard.
|HCCA       | NOT COMPATIBLE  | The Retro does not have an HCCA although something similar is in the works.
|Interrupts | PART COMPATIBLE | The Retro has no interrupt service routines defined.  It would need to provide ISR support.
|ROM Control| UNKNOWN         | There is an IO register on the NABU that allows to disable ROM.

## Way Forward

We can look for some basic supported functionality by implimenting library functions for the VDP and Joysticks as the Retro does have both of these available.

### Interrupts

The retro does not currently use interrupts in the BIOS and CP/M.  This means we can use them in the Retro port of NABU-LIB.

_side note: The more I think about this, the more I think we just need our own library.  Porting might not cover what we need to do here._

### VDP

Besides the VSYNC interrupt which would need to be dealt with in tandem with the work required to impliment interrupts, the rest of the functions are straightforward to impliment.

NABU-LIB Has this function to setup the VDP interrupt service routine:

```c
  void vdp_enableVDPReadyInt() {

    NABU_DisableInterrupts();

    __asm

      // Video Frame Sync
      ld hl, _waitVdpISR;
      ld (INTERUPT_VECTOR_MAP_ADDRESS + 6), hl;

    __endasm;

    _ORIGINAL_INT_MASK |= INT_MASK_VDP;

    ayWrite(IOPORTA, _ORIGINAL_INT_MASK);

    NABU_EnableInterrupts();

    _vdpInterruptEnabled = true;
    vdp_setRegister(1, _vdpReg1Val | 0b00100000 );   
  }
```

The actual ISR looks like this:

```c
  void waitVdpISR() __naked {

    __asm

      push  af;      

    // _vdpStatus = IO_VDPLATCH;
      in	a, (_IO_VDPLATCH)
      ld	(_vdpStatusRegVal), a

    // _vdpReady = true;
      ld  a, 0x01
      ld  (_vdpIsReady), a

      pop af;      

      ei;
      reti;

    __endasm;
  }
```

Commentary:

* The variable _vdpIsReady is a global variable defined thusly `volatile bool    vdpIsReady      = false;`.  Looking at z88dk docs, when inline assembly is used variables in C must be prefixed with an `_` to be referenced in the assembly.  So `ld (_vdpIsReady), a` in the inline assembly sets the value of the global to 1 or true.
* All the ISR does is set the vdpIsReady global to true.
* The `enableVdpIsReady()` function sets up the ISR into the ISR Vector table.  On the nabu this is at 0xFF00 + 6  = 0xFF06.
* The way interrupts are set on the NABU is by writing to PORT A on the PSG.  The bus that connects from PORTA of PSG is then fed through a logic decoder circuit which feeds to the /INT pin on the Z80 CPU.  This circuit contains an LS348 8input, 3output priority encoder.  The VDP signal is the 4th highest priority after HCCAR, HCCAT and KBD.
*  The code to actually enable the VDP Interrupts on the VDP is this line in the enable function. `vdp_setRegister(1, _vdpReg1Val | 0b00100000 );`  In other words turn on the INTERRUPTS enable bit on VDP REGISTER 1.
* There is code to disalbe this bit too.

To get this working on the Retro I need to know or decide on some things:

1. Is the IM2 interrupt vector table configured anywhere?  How do I tell the RETRO where and how to handle the VDP interrupts?
  1. Or this done in some other way?
2. Maybe we just want to POLL the VDP for it's interrupt status.  If that's the case, we can stub out all the VDP ready stuff when compling for the Retro and just run a busy loop waiting for VDP Interrupt status bit to be set.  On the retro, we just don't add the header shunt on the VDP interrupt line.

### Joysticks

The NABU joystick library will need to be rewritten to support the Retro IO layout which is a trival excersize. 

### Keyboard

Without a Keyboard it could be quite difficult to port games from one system to the other.  If that's the goal of this system, we "could" engage in some kind of serial interface hackery, but that would be less than ideal.  Games that rely on keyboard input usually require low level / low lag input response.

Suggestion here is to determine the purpose of the library port and understand what we want to do with it.  Ultimately our choices are limited.

1. Support keybaord input via Serial.
2. Support user input via Joystick only.
3. Some kind of hybrid solution.

### HCCA

The NABU has a fully functioning network access to storage solution which appears to be provided by NABU-LIB.  I haven't used it so don't know how it works specifically.  If the Retro CPM environment is being upgraded to support DISK over Network capability (HCAP) then I "guess" this is something we can do.

The Retro has a filesystem! So we just need to impliment something that lets CP/M take care of file IO.

### ROM

NABU-LIB has this snippet in its init function:

```c
  // Turn off the rom
  IO_CONTROL = CONTROL_ROMSEL | CONTROL_VDOBUF;
```

According to the NABU Technical Manual this to say about it:

```text
ROM 
A 4K byte ROM (Read Only Memory) contains the bootstrap software. Upon 
reset, this software performs a minimal self test on the processor board and initializes the system. It also contains software which can communicate with the HC.CA or a floppy disk to allow for bootstrapping. 

The ROM may be selected or deselected under software control. When selected, any memory read request to addresses 0000 - OFFF hex will be directed to the ROM; write requests to these addresses will go to RAM.
```

I "assume" this means that we can dynamically disable the rom and re-enable it later or it's automatically re-enabled during a warm boot.  The concept of Warm Boot in NABU is only applicable to CP/M.  Everything else in Nabu Land appears to require a reset.
(happy to be corrected on this point.)

