# Compiled with standard options (add -O3 for optimisation)
# Add -DTEST_SCHNAPSEN during development
# drmemory.exe -- appName.exe to catch memory leaks (add in -ggdb and remove -O3 (also -flto ))
# Add -pg for use with gprof (remember to remove -O3 otherwise function inlining issues)
# Had to remove -pedantic in order to allow binary #define's
GCC = gcc -std=c99 -Wall -Werror -O3

UPDATE_STATE_H_C = updateState.h updateState.c getMoves.h getMoves.c moveGroup.h moveGroup.c state.h state.c move.h move.c pile.h pile.c card.h card.c

UPDATE_STATE_C = updateState.c getMoves.c moveGroup.c state.c move.c pile.c card.c

default: schnapsen


card: card.h card.c
	$(GCC) -DTEST_CARD -o card card.c
	./card.exe

pile: pile.h pile.c card.h card.c
	$(GCC) -DTEST_PILE -o pile pile.c card.c
	./pile.exe

move: move.h move.c pile.h pile.c card.h card.c
	$(GCC) -DTEST_MOVE -o move move.c pile.c card.c
	./move.exe

state: state.h state.c move.h move.c pile.h pile.c card.h card.c
	$(GCC) -DTEST_STATE -o state state.c move.c pile.c card.c
	./state.exe

moveGroup: moveGroup.h moveGroup.c state.h state.c move.h move.c pile.h pile.c card.h card.c
	$(GCC) -DTEST_MOVEGROUP -o moveGroup moveGroup.c state.c move.c pile.c card.c
	./moveGroup.exe

getMoves: getMoves.h getMoves.c moveGroup.h moveGroup.c state.h state.c move.h move.c pile.h pile.c card.h card.c
	$(GCC) -DTEST_GETMOVES -o getMoves getMoves.c moveGroup.c state.c move.c pile.c card.c
	./getMoves.exe

updateState: $(UPDATE_STATE_H_C)
	$(GCC) -DTEST_UPDATESTATE -o uState $(UPDATE_STATE_C)
	./uState.exe

display: display.h display.c $(UPDATE_STATE_H_C)
	$(GCC) -DTEST_DISPLAY -o display display.c $(UPDATE_STATE_C)
	./display.exe

permsAndCombs: permsAndCombs.h permsAndCombs.c display.h display.c $(UPDATE_STATE_H_C)
	$(GCC) -DTEST_PERMSANDCOMBS -o permsAndCombs permsAndCombs.c display.c $(UPDATE_STATE_C)
	./permsAndCombs.exe

sims: sims.h sims.c permsAndCombs.h permsAndCombs.c display.h display.c $(UPDATE_STATE_H_C)
	$(GCC) -DTEST_SIMS -o sims sims.c permsAndCombs.c display.c $(UPDATE_STATE_C)
	./sims.exe

gameNodeMemory: gameNodeMemory.c gameNodeMemory.h sims.h sims.c permsAndCombs.h permsAndCombs.c display.h display.c $(UPDATE_STATE_H_C)
	$(GCC) -DTEST_GAMENODEMEMORY -o gameNodeMemory gameNodeMemory.c sims.c permsAndCombs.c display.c $(UPDATE_STATE_C)
	./gameNodeMemory.exe

gameNode: gameNode.c gameNode.h gameNodeMemory.c gameNodeMemory.h sims.h sims.c permsAndCombs.h permsAndCombs.c display.h display.c $(UPDATE_STATE_H_C)
	$(GCC) -DTEST_GAMENODE -o gameNode gameNode.c gameNodeMemory.c sims.c permsAndCombs.c display.c $(UPDATE_STATE_C)
	./gameNode.exe

minimax: minimax.c minimax.h gameNode.c gameNode.h gameNodeMemory.c gameNodeMemory.h sims.h sims.c permsAndCombs.h permsAndCombs.c display.h display.c $(UPDATE_STATE_H_C)
	$(GCC) -DTEST_MINIMAX -o minimax minimax.c gameNode.c gameNodeMemory.c sims.c permsAndCombs.c display.c $(UPDATE_STATE_C)
	./minimax.exe

agents: agents.h agents.c gameNode.c gameNode.h gameNodeMemory.c gameNodeMemory.h sims.h sims.c permsAndCombs.h permsAndCombs.c display.h display.c $(UPDATE_STATE_H_C)
	$(GCC) -DTEST_AGENTS  -o agents agents.c minimax.c gameNode.c gameNodeMemory.c sims.c permsAndCombs.c display.c $(UPDATE_STATE_C)
	./agents.exe

play: play.h play.c agents.h agents.c minimax.c minimax.h gameNode.c gameNode.h gameNodeMemory.c gameNodeMemory.h sims.h sims.c permsAndCombs.h permsAndCombs.c display.h display.c $(UPDATE_STATE_H_C)
	$(GCC) -DTEST_PLAY -o play play.c agents.c minimax.c gameNode.c gameNodeMemory.c sims.c permsAndCombs.c display.c $(UPDATE_STATE_C) 
	./play.exe

control: control.h control.c play.h play.c agents.h agents.c minimax.c minimax.h gameNode.c gameNode.h gameNodeMemory.c gameNodeMemory.h sims.h sims.c permsAndCombs.h permsAndCombs.c display.h display.c $(UPDATE_STATE_H_C)
	$(GCC) -DTEST_CONTROL -o control control.c play.c agents.c minimax.c gameNode.c gameNodeMemory.c sims.c permsAndCombs.c display.c $(UPDATE_STATE_C)
	./control.exe

schnapsen: schnapsen.h schnapsen.c control.h control.c play.h play.c agents.h agents.c minimax.c minimax.h gameNode.c gameNode.h gameNodeMemory.c gameNodeMemory.h sims.h sims.c permsAndCombs.h permsAndCombs.c display.h display.c $(UPDATE_STATE_H_C)
	$(GCC) -o schnapsen schnapsen.c control.c play.c agents.c minimax.c gameNode.c gameNodeMemory.c sims.c permsAndCombs.c display.c $(UPDATE_STATE_C)
	./schnapsen.exe





profiler: profiler.c control.h control.c play.h play.c agents.h agents.c minimax.c minimax.h gameNode.c gameNode.h gameNodeMemory.c gameNodeMemory.h sims.h sims.c permsAndCombs.h permsAndCombs.c display.h display.c $(UPDATE_STATE_H_C)
	$(GCC) -o profiler profiler.c control.c play.c agents.c minimax.c gameNode.c gameNodeMemory.c sims.c permsAndCombs.c display.c $(UPDATE_STATE_C)
	./profiler.exe
#gprof profiler.exe > profile.txt

monitor_agents: monitor_agents.c monitor_agents.h minimax.c minimax.h gameNode.c gameNode.h gameNodeMemory.c gameNodeMemory.h sims.h sims.c permsAndCombs.h permsAndCombs.c display.h display.c $(UPDATE_STATE_H_C)
	$(GCC) -o monitor_agents monitor_agents.c minimax.c gameNode.c gameNodeMemory.c sims.c permsAndCombs.c display.c $(UPDATE_STATE_C)
	./monitor_agents.exe

sims_variance: sims_variance.c agents.h agents.c minimax.c minimax.h gameNode.c gameNode.h gameNodeMemory.c gameNodeMemory.h sims.h sims.c permsAndCombs.h permsAndCombs.c display.h display.c $(UPDATE_STATE_H_C)
	$(GCC) -o sims_variance sims_variance.c agents.c minimax.c gameNode.c gameNodeMemory.c sims.c permsAndCombs.c display.c $(UPDATE_STATE_C)
	./sims_variance.exe

cpuTime:
	$(GCC) -c cpuTime.c




clean:
	del *.exe