# version
Automatic version control engine

add to your Makefile string
`VERSION=$(shell ./version.sh | head -n 1 )`

MAJOR - manually change
MINOR - add free description for generate next
BUILD - changes automatically

just remove the `version` file for generate new one 
