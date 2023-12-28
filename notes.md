- `SequencerEventQueue` only handles "low level events"
  - its main purpose is for scheduling corresponding noteOff events for each noteOn event

- create higher level constructs that are more useful
  - these can use `SequencerEventQueue` to do low level things

- think about being able to quickly and easily "spin up" sequences
  - each sequence is an object
  - has properties:
    - step size -- quarter note, 8th note, etc.
    - sequence length -- 1 bar, 1 quarter note, etc.
    - array of events -- each elt of array corresponds to one step of the sequence
    - play head
    - loop behavior
      - loop or one shot
      - how many times to loop
      - loop points?
      - loop direction?
    - other stuff?

- what if sequences can be nested?
  - example:
    - have a sequence with step size quarter note, length 1 bar
    - each elt of the array is a sequence with step size 16th note, length quarter note

- once a sequence has been created and "registered" or something, it will automatically begin playing
  - it has some sort of current play head that increments with each tick (or whatever)
  - the sequence has a tick-like function
    - takes the current global tick as input
    - like an old factory where all the machines are belt driven by the same belt

- can have various types of events:
  - play a note
  - play a roll of notes
  - modify other events
    - transpose note up or down
    - move events to other tracks
    - silence events
  - reverse playhead
  - modify playhead
  - record a sequence of notes from somewhere and paste it elsewhere
    - sort of like a buffer shuffler for midi notes
  - conditionals
    - trigger if some condition is met
    - trigger if some other notes will also trigger
  - probability
  - arpeggio, chord, scale related stuff
  - begin playing some other sequence
    - this could be used to implement the "nested sequences" idea above
    - sequences don't have to literally be nested
    - outer sequence triggers inner sequence to begin playing
    - inner sequence would probably be a one shot sequence
  - generate an entire new sequence

- i guess what we will probably want is some sort of "global" vector of all sequences
    - each tick, do something like: 
      - `for seq in seqVector: seq.onTick(curTick)`
    - similar to video game development





