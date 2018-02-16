unit PIT;

interface

uses crt, dos;

{$F+}

const TIMERINTR = 8;
       PIT_FREQ = $1234DD;

var BIOSTimerHandler : procedure;
    clock_ticks, counter : longint;

procedure SetTimer(TimerHandler : pointer; frequency : word);
procedure CloseTimer;

implementation
{ The clock_ticks variable will keep track of how many cycles the PIT has
had, it'll be intialised to 0. The counter variable will hold the new
channel 0 counter value. We'll also be adding this number to clock_ticks
every time our handler is called.                                         }

procedure SetTimer(TimerHandler : pointer; frequency : word);
begin

  { Do some initialization }
  clock_ticks := 0;
  counter := $1234DD div frequency;

  { Store the current BIOS handler and set up our own }
  GetIntVec(TIMERINTR, @BIOSTimerHandler);
  SetIntVec(TIMERINTR, TimerHandler);

  { Set the PIT channel 0 frequency }
  Port[$43] := $34;
  Port[$40] := counter mod 256;
  Port[$40] := counter div 256;
end;

procedure CloseTimer;
begin
  { Restore the normal clock frequency }
  Port[$43] := $34;
  Port[$40] := 0;
  Port[$40] := 0;

  { Restore the normal ticker handler }
  SetIntVec(TIMERINTR, @BIOSTimerHandler);
end;


end.

