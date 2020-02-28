# Simple Energy Managememt

## Update Logs

### Jan. 29th 2020
Node will send "Check Alive" message to its neighbor in every round.

Only all nodes receives messages from all its neighbors, can it go to next
round.

### Feb. 3rd 2020
A newly designed 3\*3 grid was built.

The nodes which are less used for processing task message can be switched to
semi-active mode.

Yet to find bugs :-(

### Feb. 4th 2020
Merged some functions. It's time to do some practical things (hopefully)

### Feb. 28th 2020
* How to switch mode
  * Switch mode by frequency (idle period)
  * Switch mode by probability (flip coin)
* 3 task messages forwarding randomly

