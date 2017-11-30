#include <vector>
#include "rusttypes.h"
#include <utility>
#include <iostream>
#include <exception>
#include <cstdlib>

using namespace std;
namespace monopoly {
    typedef u32 Money;
    typedef u8 PlayerID;
    typedef u8 FieldID;
    constexpr PlayerID nobody = 0;
    namespace fields {
        class Plot
        {
        public:
            bool hasHouse;
            PlayerID ownerID;
            Plot()
            {
                this->hasHouse=false;
                this->ownerID=nobody;
            };
            Money rent() const
            {
                if(this->hasHouse)
                {
                    return 500;
                }
                else
                {
                    return 2000;
                }
            }
            Money upgradePrice() const
            {
                if(this->hasHouse)
                {
                    return 4000;
                }
                else
                {
                    return 1000;
                }
            }
        };
        class Utility
        {
        public:
            Money fee;
            PlayerID ownerID;
            Utility(Money fee)
            {
                this->fee=fee;
                this->ownerID=nobody;
            }
        };
        class Luck
        {
        public:
            Money prize;
            Luck(Money prize)
            {
                this->prize=prize;
            }
        };
    }
    enum FieldType {
        Plot=0,
        Utility,
        Luck,
        NFieldType
    };
    class Field
    {
    public:
        enum FieldType type;
        union {
            fields::Plot plot;
            fields::Utility utility;
            fields::Luck luck;
        };
        Field(FieldType type)
        {
            
        }
    };
    enum PlayerAction {
        Upgrade=0,
        Skip,
        NPlayerActionType,
    };
    class Game;
    class IPlayer
    {
    public:
        Money money;
        FieldID field;
        IPlayer()
        {
            this->money = 0;
            this->field = 0;
        }
        ~IPlayer(){}
        virtual enum PlayerAction act(const Game &gamestate);
    };
    class Game {
    public:
        vector<IPlayer> players;
        vector<PlayerID> activePlayers;
        vector<Field> fields;
        u8 activePlayer;
        u64 tick;
        Game(vector<IPlayer> players)
        {
            this->activePlayer = nobody;
            this->players = players;
            this->tick = 0;
            this->activePlayers.reserve(this->players.size());
            for(PlayerID i=0; i<this->players.size();)
            {
                i++;
                this->activePlayers.push_back(i);
            }
        }
        bool isGameOver() const
        {
            //empty playing field || 0 players || only one player remaining
            return (this->fields.size() == 0 || this->activePlayers.size() <= 1);
        }
        void step()
        {
            //assumes having at least one active player
            this->activePlayer += 1;
            if(this->activePlayer==nobody)
            {
                this->activePlayer+=1;
            }
            IPlayer &player = this->players[-1+this->activePlayer];
            {
                u8 roll = rand() % 6 + 1;
                player.field += roll;
                player.field %= this->fields.size();
            }
            const Field &field = this->fields[player.field];
            bool bankrupt;
            if(field.type==Plot)
            {
                if(field.plot.ownerID!=nobody)
                {
                    Money rent = field.plot.rent();
                    bankrupt = rent > player.money;
                    if(!bankrupt)
                    {
                        player.money -= rent;
                        players[-1+field.plot.ownerID].money += rent;
                    }
                    else
                    {
                        players[-1+field.plot.ownerID].money += player.money;
                    }
                }
            }
            if(field.type==Utility)
            {
                Money fee = field.utility.fee;
                bankrupt = fee > player.money;
                if(!bankrupt)
                {
                    player.money -= field.utility.fee;
                }
            }
            if(!bankrupt)
            {
                player.act(*this);
            }
            else
            {
                this->activePlayers.erase(-1 +
                                          this->activePlayers.begin() +
                                          this->activePlayer);
                for(FieldID i = 0; i < this->fields.size(); i++)
                {
                    Field &field = this->fields[i];
                    if(field.type == Plot && field.plot.ownerID == this->activePlayer)
                    {
                        field.plot.ownerID = 0;
                    }
                }
            }
        }
        PlayerID run()
        {
            while(!this->isGameOver())
            {
                this->step();
                this->tick+=1;
            }
            return this->activePlayer;
        }
    };
    namespace players
    {
        class Greedy : IPlayer
        {
            ~Greedy(){}
            enum PlayerAction act(const Game &gamestate)
            {
                return Upgrade;
            }
        };
        class Careful : IPlayer
        {
            ~Careful(){}
            enum PlayerAction act(const Game &gamestate)
            {
                const auto field = gamestate.fields[this->field];
                if(field.type==Plot && field.plot.upgradePrice() < this->money/2)
                {
                    return Upgrade;
                }
                else
                {
                    return Skip;
                }
            }
        };
        class Tactical : IPlayer
        {
            ~Tactical(){}
            enum PlayerAction act(const Game &gamestate)
            {
                if(gamestate.tick&1/*because we know binary*/)
                {
                    const auto field = gamestate.fields[this->field];
                    if(field.type==Plot)
                    {
                        return Upgrade;
                    }
                    else
                    {
                        return Skip;
                    }
                }
                else
                {
                    return Skip;
                }
            }
        };
    }
    namespace exceptions {
        class TooShort : exception
        {
        public:
            usize gotLength;
            TooShort(usize got)
            {
                this->gotLength = got;
            }
        };
    }
    void loadFieldsFromStream(Game &game, istream &stream)
    {
        //not in Game because they are not on the same abstraction level
        usize length;
        int ifieldtype;
        stream >> length;
        game.fields.reserve(length);
        while(length>0 && stream>>ifieldtype)
        {
            FieldType fieldtype = (FieldType)ifieldtype;
            game.fields.push_back(Field(fieldtype));
        }
        if(length>0)
        {
            throw exceptions::TooShort(length);
        }
    }
}
