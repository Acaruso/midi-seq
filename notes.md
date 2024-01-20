- `MidiQueue` only handles "low level events"
  - its main purpose is for scheduling corresponding noteOff events for each noteOn event

- create higher level constructs that are more useful
  - these can use `MidiQueue` to do low level things

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

- chord voice leading ear training idea:
  - pick a key -- ie a root note
    - assume always major key
  - pick a range of notes
  - pick a starting triad in that key and range
  - to transition to next chord:
    - randomly pick another chord in the key
    - generate all three possible inversions of that chord
    - find which inversion has the closest voice leading to the current chord
      - to do this, take the following measurements:
        - low_diff = abs(curChord.lowest - nextChord.lowest)
        - mid_diff = abs(curChord.mid - nextChord.mid)
        - hi_diff = abs(curChord.hi - nextChord.hi)
        - total = low_diff + mid_diff + hi_diff
  - this will not only help with ear training, it will help with learning all the various chord shapes of a key

- guitar strings to MIDI note numbers:
  - 40 == E1
  - 45 == A1
  - 50 == D1
  - 55 == G2
  - 59 == B2
  - 64 == E3



























