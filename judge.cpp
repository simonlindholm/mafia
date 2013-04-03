#include <ctime>
#include <iostream>
#include <typeinfo>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include "api.h"
using namespace std;

// (int)(rand() / (RAND_MAX+1.0) * n);
class ASBot;

#define ALLi for (int i = 0; i < N; ++i)

namespace {

typedef Bot::Role Role;
const Role
	MAFIA = Bot::MAFIA,
	VILLAGE = Bot::VILLAGE,
	DETECTIVE = Bot::DETECTIVE;

struct Player {
	::Role role;
	bool dead;
	Bot* bot;
	Player() : dead(false) {}
};
int N;
int nmafia = 0;
int nplayers = 0;
vector<Player> players;

bool valid(int target, bool canBeDead = false) {
	return target >= 0 && target < N && (canBeDead || !players[target].dead);
}

int playerMakingClaim;
void globalMakeClaim(int player, double prob, Role role) {
	assert(valid(player, true));
	assert(prob >= 0 && prob <= 1);
	assert(role >= 0 && role < Bot::N_ROLES);
	ALLi {
		Player& pl = players[i];
		if (i == playerMakingClaim) continue;
		if (pl.dead) continue;
		pl.bot->say(playerMakingClaim, player, prob, role);
	}
}

void checkDone() {
	if (nmafia && nmafia*2 < nplayers)
		return;
	cout << "Winner: " << (nmafia ? "Mafia" : "Town") << "! ("
		<< (nplayers - nmafia) << " vs " << nmafia << endl;
	cout << "Winning team: ";
	ALLi {
		if ((players[i].role == MAFIA) == !!nmafia)
			cout << typeid(players[i].bot).name() << ' ';
	}
	cout << endl;
	exit(0);
}

void kill(int die) {
	players[die].dead = true;
	--nplayers;
	if (players[die].role == MAFIA) --nmafia;
	checkDone();
}

void dayPhase() {
	int rounds = 5;
	for (int r = 0; r < rounds; ++r) {
		ALLi {
			Player& pl = players[i];
			if (pl.dead) continue;
			playerMakingClaim = i;
			pl.bot->round(i, rounds-r, &globalMakeClaim);
		}
	}
	vector<int> votes(N, 0), pvotes, choices;
	int max = 0;
	ALLi {
		Player& pl = players[i];
		if (pl.dead) continue;
		int target = pl.bot->vote();
		assert(valid(target));
		pvotes.push_back(target);
		if (++votes[target] > max) max = votes[target];
	}
	ALLi if (votes[i] == max) choices.push_back(i);
	random_shuffle(choices.begin(), choices.end());
	int die = choices[0];
	kill(die);
	ALLi {
		Player& pl = players[i];
		if (pl.dead) continue;
		pl.bot->nightFall(&pvotes[0], die, (players[die].role == MAFIA));
	}
}
void nightPhase() {
	vector<int> choices;
	ALLi {
		Player& pl = players[i];
		if (pl.dead) continue;
		if (pl.role != VILLAGE) {
			int target = pl.bot->nightTarget();
			assert(valid(target));
			if (pl.role == MAFIA)
				choices.push_back(target);
			else
				pl.bot->nightResult(players[target].role == MAFIA);
		}
	}
	random_shuffle(choices.begin(), choices.end());
	int die = choices[0];
	kill(die);
	ALLi {
		Player& pl = players[i];
		if (pl.dead) continue;
		pl.bot->dayStart(die);
	}
}

int main2() {
	int seed = (int)time(0);
	srand(seed);
	players.push_back(new ASBot);
	players.push_back(new ASBot);
	players.push_back(new ASBot);
	N = (int)players.size();
	random_shuffle(players.begin(), players.end());
	Role roles[] = {
		MAFIA,
		VILLAGE,
		VILLAGE,
		DETECTIVE,
		VILLAGE,
		MAFIA,
		VILLAGE,
		VILLAGE,
		VILLAGE,
		MAFIA,
		VILLAGE,
		VILLAGE,
		VILLAGE,
		VILLAGE
	};
	int roleCount[Bot::N_ROLES] = {};
	ALLi {
		players[i].role = roles[i];
		++roleCount[roles[i]];
		++nplayers;
		if (roles[i] == MAFIA)
			++nmafia;
	}
	random_shuffle(players.begin(), players.end());

	vector<int> theMafia;
	ALLi {
		if (roles[i] == MAFIA)
			theMafia.push_back(i);
	}

	ALLi {
		Player& p = players[i];
		Bot* nb = p.bot->replaceSelfWith(p.role);
		if (nb) p.bot = nb;
		p.bot->init(N, roleCount, i, p.role, (p.role == MAFIA ? &theMafia[0] : 0));
	}

	while (true) {
		dayPhase();
		nightPhase();
	}

	return 0;
}

}

int main() {
	return main2();
}
