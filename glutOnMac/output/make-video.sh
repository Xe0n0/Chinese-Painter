#!/bin/bash
ffmpeg -f image2 -i %06d.png -r 24 -s 1024x768 solar-system.avi
