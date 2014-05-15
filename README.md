Livecut
=======

Livecut is based on [BBCut](http://www.cus.cam.ac.uk/~nc272/papers/pdfs/bbcutlib.pdf) the BreakBeat Cutting library written by [Nick Collins](http://www.cus.cam.ac.uk/~nc272/) for [SuperCollider](http://supercollider.sourceforge.net/)

The BBCut Library began out of work on an algorithm to simulate the automatic cutting of breakbeats in the style of early jungle or drum and bass, Nick Collins.

Livecut is only a small subset of what is possible with BBCut, but as it is available as a VST plugin, it is much easier to start experimenting with it.

It is a live beat-slicer but instead of manipulating equal chunks of audio like most beatslicer do, it works on the notion of audio *cuts* whose length and number of repetition depends on the context and the cutting procedure. 
*Cuts* are organized in *blocks* which then form a *phrase*. see Image below. And each phrase can be ended by a *roll* or *fill*.
