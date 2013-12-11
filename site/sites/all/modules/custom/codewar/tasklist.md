1: build canvas and js for processing turns at code/%/run where % == node1*node2, need to add check for access. calls codewar\_run\_code, which should populate cache values and start js process which calls code/%/step for each bot. Change caching and session variable storage to check to allow user to run bot without access checks

2: alter call\_sys\_func in code\_bot.inc to return value based on function, alter handle\_bot\_turns in code\_map.inc to process this returned result and handle the turn, return a result json for the processing JS to move and alter the bot.

3: start building stats system to record the number of wins/losses/etc. for each bot

4: add icon changing ability to code

5: improve icon editor

6: theming

###thoughts on battle system###
can they come to a draw?

kills/round, kills/life

rounds vs multiple enemies

kills by weapon