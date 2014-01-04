~~-1: add public/private flag for bots~~

~~0: make control landing page for bot, with links to run, edit icon, edit attributes, etc.~~

~~0.1: challenge form for bot vs. -- one one side is current bot (with versions later?) and other is drop down (maybe chosen select thing?) with available bots. Maybe auto-complete with bot name?~~

~~.1.1: definitely make forms auto-complete, (with some kind of add/delete form part... maybe using chosen multi-delete)~~

.1.2: add example bot boolean and add example bots to chosen dropdown list

.1.3: add description of bot to content type, add to edit or bot description page

~~0.2: add back to edit button (or maybe build a menu system?) for the edit code page and icons page~~

.3: build versioning system for code save -- with checkbox and auto-versioning on code page. Triggered on save, iterates version (look up drupal revision control) and auto-counting. VS form should only display version info for current bot (should be easy, right?) enemy bots are always latest version

1: build canvas and js for processing turns at code/%/run where % == node1*node2, need to add check for access. calls codewar\_run\_code, which should populate cache values and start js process which calls code/%/step for each bot. Change caching and session variable storage to check to allow user to run bot without access checks

2: alter call\_sys\_func in code\_bot.inc to return value based on function, alter handle\_bot\_turns in code\_map.inc to process this returned result and handle the turn, return a result json for the processing JS to move and alter the bot.

3: start building stats system to record the number of wins/losses/etc. for each bot

3a: build stats page

4: add icon changing ability to code

5: improve icon editor

6: theming

7: Alter bot home page to allow for both owner view (with links to edit, etc.) and visitor view (with stats, etc.)

###thoughts on battle system###
can they come to a draw?

kills/round, kills/life

rounds vs multiple enemies

kills by weapon

kills, deaths, rounds, wins, losses, draws, kills by ranged, kills by mid-range, kills by mine, kills by bomb, kills by sword (?)