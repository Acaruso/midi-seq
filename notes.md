- think of sequencerEventQueue as being only for sequencing/scheduling "low level events"
  - main purpose is for scheduling corresponding noteOff events for each noteOn event

- create other higher level constructs that are more useful for musical sequencing
  - these will call methods on sequencerEventQueue to do various low level stuff

- think about being able to quickly and easily "spin up" sequences
- each sequence is an object
- has properties:
  - step size -- quarter note, 8th note, etc.
  - total length of sequence -- 1 bar, 1 quarter note, etc.
  - array of events where each elt of array corresponds to one step

- what if sequences can be nested?
  - example:
    - have a sequence with step size quarter note, length 1 bar
    - each elt of the array is a sequence with step size 16th note, length quarter note

- once a sequence has been created and "registered" or something, it will automatically begin playing
  - it has some sort of current play head that increments with each tick (or whatever)

- can have various types of events
  - play a note
  - play a roll of notes
  - modify other events
    - transpose note up or down
    - move events to other tracks
    - silence events
  - reverse playhead
  - modify playhead
  - record a sequence of notes from somewhere and paste it elsewhere
    - sort of a buffer shuffler for midi notes
  - conditionals
    - trigger if some condition is met
    - trigger if some other notes will also trigger
  - probability
  - arpeggio, chord, scale related stuff






