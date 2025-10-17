# dotman

dotfile manager

## Map

- [x] Read from config file
- [x] Write to config file
- [ ] CMD:
  - [ ] add: name target source
    - add a dotfile to the cfg and create the link.
    - [ ] Check before creating a link
  - [ ] del: name
    - deleted the dotfile from the cfg and destroy the link by the given name
  - [ ] list:
    - list all of the links. show if it's created, backuped etc.
  - [ ] sync:
    - create the missing links
  - [ ] init:
    - create the home folder if missing, clone the git repo and sync.
    - check for every edge case.
  - [ ] backup: name
    - backup given dotfile
  - [ ] help:
    - show help message
  - [ ] version:
    - show the version
