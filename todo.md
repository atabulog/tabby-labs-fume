# Tasks

## Standup flash
1. Pull flash code from ambient and try to setup flash with encryption

## HTTPS based network config form
1. Steal ambient networking stuff, and get an AP and STA network on fume.
1. Get form data tied into flash and have it connect to the network
1. Consider some way to default back to AP if a STA connection cannot be made with flash data
1. Upon successful form submission, the page should reflect the device's new ip address. 

## Standup a fan controller
1. Create a post that sets the fan between 0-100
1. Create a post that sets the reset speed (default is 50)
1. Create a get that reports the current fan speed

## New config
1. Change IO Pins to match new config. 
    - PWM -> 15
    - Tach -> 12
    - SW -> 13
    - DT -> 12
    - CLK -> 14
1. add pulse counter logic to the tach reading. Setup a task to measure this at 1Hz or just sample in main.