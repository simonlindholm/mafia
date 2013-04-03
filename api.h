class Bot {
	public:
		enum Role {
			MAFIA,
			VILLAGE,
			DETECTIVE,
			N_ROLES
		};
        // Skicka en anklagan mot någon
		typedef void (*ClaimF)(int, double, Role);
		virtual ~Bot() = 0;

		virtual Bot* replaceSelfWith(Role role) const { return 0; }

        // Initialisera bot
	    virtual void init(int N, const int nroles[], int index, Role role, const int mafia[]) = 0;

        // Vem rollen vill rösta på
		virtual int nightTarget() = 0;

        // Detektiven får information om personen
		virtual void nightResult(bool isMafia) = 0;

        // Dagen startar; vem dog inatt?
		virtual void dayStart(int died) = 0;

        // Dagen är slut: vem röstade varje person på, vem dog, var denne maffia?
		virtual void nightFall(const int votes[], int died, bool wasMafia) = 0;

        // Någon sa någonting! Vad?
		virtual void say(int player, int target, double prob, Role claim) = 0;

        // Det är starten på din runda :-) Make your claims!
		virtual void round(int round, int left, ClaimF callback) = 0;

        // Rösta om hängning
		virtual int vote() = 0;
};
