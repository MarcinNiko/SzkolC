#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <ctime>
#ifdef __linux__ 
#elif _WIN32
#include <windows.h>
#endif

const int suits_count = 4;
const int ranks_count = 13;
const int sleep_time = 2;
const int player_index = 4;
std::string suits[suits_count];
std::string ranks[ranks_count];

void press_Enter()
{
	printf("\nPress Enter to continue.\n");
	while (getchar() != '\n');
}

void _sleep(int time)
{
#ifdef __linux__ 
	sleep(time);
#elif _WIN32
	Sleep(time);
#endif
}

class Card
{
public:
	int suit;
	int rank;
};

int compareCards(const void *card1, const void *card2)
{
	return (*(Card *)card1).rank - (*(Card *)card2).rank;
}

class Deck
{

private:
	int top;
	static const int card_tally = 52;

	Card cards[card_tally];

public:
	Deck()
	{
		for (int i = 0; i < suits_count; i++)
		{
			for (int j = 0; j < ranks_count; j++)
			{
				cards[i * ranks_count + j].suit = i;
				cards[i * ranks_count + j].rank = j;
			}
		}
		suits[0] = "D";
		suits[1] = "S";
		suits[2] = "H";
		suits[3] = "C";

		ranks[0] = "2";
		ranks[1] = "3";
		ranks[2] = "4";
		ranks[3] = "5";
		ranks[4] = "6";
		ranks[5] = "7";
		ranks[6] = "8";
		ranks[7] = "9";
		ranks[8] = "T";
		ranks[9] = "J";
		ranks[10] = "Q";
		ranks[11] = "K";
		ranks[12] = "A";
	}

	void print()
	{
		std::cout << "Printing the deck..." << std::endl;
		_sleep(1);
		for (int i = 0; i < card_tally; i++)
		{
			std::cout << ranks[cards[i].rank] << suits[cards[i].suit] << std::endl;
		}
		printf("\n");
	}

	void shuffle()
	{
		top = card_tally - 1;

		for (int i = 0; i < suits_count; i++)
		{
			for (int j = 0; j < ranks_count; j++)
			{
				cards[i * ranks_count + j].suit = i;
				cards[i * ranks_count + j].rank = j;
			}
		}

		int x;
		Card tempCard;
		for (int i = 0; i < card_tally; i++)
		{
			x = rand() % card_tally;
			tempCard = cards[i];
			cards[i] = cards[x];
			cards[x] = tempCard;
		}
	}

	Card hitme()
	{
		top--;
		return cards[top + 1];
	}
};

class Player
{
public:
	std::string name;
	int money;
	Card cards[2];
	bool playing;
	int round;
	int goodToGo;
    bool IfAllIn = false;
	int player_input[1] = {0};
	int player_socket = 0;
};

class PokerGame
{
public:

	char* start(const std::string &name)
	{
		for (int i = 0; i < players_count; i++)
		{
			players[i].money = 1000;
			players[i].playing = true;
		}

		char* arrOfNames[] = {"Wojciech", "Tristan", "Michal", "Edyta", "name", "Kamil", "Bartek", "Jacek", "Patryk", "Norbert"};
		for (int k = 0; k < players_count; k++)
		{
			players[k].name = arrOfNames[k];
			if (k == player_index) players[player_index].name = name;
		}


		startGame();
	}


	void deal()
	{
		for (int i = 0; i < players_count; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				if (players[i].playing)
				{
					players[i].cards[j] = deck1.hitme();
				}
			}
		}
		for (int i = 0; i < 5; i++)
			tableCards[i].rank = -1;
	}

	void flop()
	{
		for (int i = 0; i < 3; i++)
			tableCards[i] = deck1.hitme();
	}

	void turn()
	{
		tableCards[3] = deck1.hitme();
	}

	void river()
	{
		tableCards[4] = deck1.hitme();
	}

	char* printTable()
	{
		using std::cout;
		using std::endl;
		using std::setw;

		cout << "------------------------------------------------------------------------------------------------------------------" << endl;
		cout << "\t  " << ((players[0].playing) ? (players[0].name) : "      ") << "         " << ((players[1].playing) ? (players[1].name) : "     ") << "           "
			<< ((players[2].playing) ? (players[2].name) : "    ") << "\t\t"<< "LEGEND:" <<endl;
		cout << "\t   $" << setw(4) << ((players[0].playing) ? (players[0].money) : 0) << "         $" << setw(4) << ((players[1].playing) ? (players[1].money) : 0)
			<< "\t           $" << setw(4) << ((players[2].playing) ? (players[2].money) : 0) << endl;
		cout << "\t     _____________________________" << "\t\t\tD - Diamonds" <<endl;
		cout << "\t    / " << ((bind == 0) ? "@" : " ") << "            " << ((bind == 1) ? "@" : " ") << "            " << ((bind == 2) ? "@" : " ") << " \\"<<"\t\t\tH - Hearts" << endl;
		if(players_count >= 8) {cout << setw(8) << ((players[6].playing) ? (players[6].name) : "      ") << "   /  ___   ___   ___   ___   ___  \\" <<"\t"<< ((players[7].playing) ? (players[7].name) : "      ") <<"\t\tC - Clubs" <<endl;}
		else {cout << "\t   /  ___   ___   ___   ___   ___  \\" << "\t\t\tC - Clubs"<< endl;}
		if ( players_count >= 8) {cout << "  $"<< setw(4) <<((players[6].playing) ? (players[6].money) : 0) << "    | | " << ((tableCards[0].rank) >= 0 ? ranks[tableCards[0].rank] : " ") << " | | " << ((tableCards[1].rank) >= 0 ? ranks[tableCards[1].rank] : " ") << " | | " << ((tableCards[2].rank) >= 0 ? ranks[tableCards[2].rank] : " ") << " | | "
			<< ((tableCards[3].rank) >= 0 ? ranks[tableCards[3].rank] : " ") << " | | " << ((tableCards[4].rank) >= 0 ? ranks[tableCards[4].rank] : " ") << " | |" << "\t$"<< setw(4) <<((players[7].playing) ? (players[7].money) : 0) << "\t\tS - Spades"<<endl;}
		else {cout << setw(4) << "\t   | | " << ((tableCards[0].rank) >= 0 ? ranks[tableCards[0].rank] : " ") << " | | " << ((tableCards[1].rank) >= 0 ? ranks[tableCards[1].rank] : " ") << " | | " << ((tableCards[2].rank) >= 0 ? ranks[tableCards[2].rank] : " ") << " | | "
			<< ((tableCards[3].rank) >= 0 ? ranks[tableCards[3].rank] : " ") << " | | " << ((tableCards[4].rank) >= 0 ? ranks[tableCards[4].rank] : " ") << " | |" <<"\t\t\tS - Spades" <<endl;}
		cout << "\t   | | " << ((tableCards[0].rank) >= 0 ? suits[tableCards[0].suit] : " ") << " | | " << ((tableCards[1].rank) >= 0 ? suits[tableCards[1].suit] : " ") << " | | " << ((tableCards[2].rank) >= 0 ? suits[tableCards[2].suit] : " ") << " | | "
			<< ((tableCards[3].rank) >= 0 ? suits[tableCards[3].suit] : " ") << " | | " << ((tableCards[4].rank) >= 0 ? suits[tableCards[4].suit] : " ") << " | |" <<"\t\t\tJ - Joker" <<endl;
		cout << "\t   | |___| |___| |___| |___| |___| |" <<"\t\t\tQ - Queen" <<endl;
		cout << "\t   |                               |" <<"\t\t\tK - King"<<endl;
		if(players_count == 10) {cout << setw(8) << ((players[8].playing) ? (players[8].name) : "      ") << "   |	       Pot = $" << setw(4) << pot << "         |" << "\t" << ((players[9].playing) ? (players[9].name) : "      ") <<"\t\tA - Ace" <<endl;}
		else {cout << "\t   |	       Pot = $" << setw(4) << pot << "         |" <<"\t\t\tA - Ace" <<endl;}
		if(players_count == 10) {cout << "  $"<< setw(4) <<((players[8].playing) ? (players[8].money) : 0) << "    \\                               /" << "\t$"<< setw(4) << ((players[9].playing) ? (players[9].money) : 0) << endl;}
		else {cout << "\t   \\                               /" << endl;}
		cout << "\t    \\_" << ((bind == 5) ? "@" : "_") << "_____________" << ((bind == 4) ? "@" : "_") << "___________" << ((bind == 3) ? "@" : "_") << "_/" << endl;
		cout << endl;
		cout << "\t  " << ((players[5].playing) ? (players[5].name) : "      ") << "          " << ((players[player_index].playing) ? (players[player_index].name) : "      ") << "         "
			<< ((players[3].playing) ? (players[3].name) : "    ") << endl;
		cout << "\t   $" << setw(4) << ((players[5].playing) ? (players[5].money) : 0) << "          $" << setw(4) << ((players[player_index].playing) ? (players[player_index].money) : 0)
			<< "\t         $" << setw(4) << ((players[3].playing) ? (players[3].money) : 0) << endl;
		cout << endl;
		if (players[player_index].round)
		{
			cout << "   Your hand:" << endl;
			cout << "    ___    ___" << endl;
			cout << "   | " << ranks[players[player_index].cards[0].rank] << " |  | " << ranks[players[player_index].cards[1].rank] << " |" << endl;
			cout << "   | " << suits[players[player_index].cards[0].suit] << " |  | " << suits[players[player_index].cards[1].suit] << " |" << endl;
			cout << "   |___|  |___|" << endl << endl;
		}

		_sleep(3);
	}

private:
	static const int players_count = 6;
	Player players[players_count];
	Deck deck1;
	int bind;
	Card tableCards[5];
	int pot, bet, picked_action, rational, betOn, winner, maxPoints, roundWinner;
	int handPoints[6];
	int bestHand[6][3];

    enum  enabled_actions {FLOP=1, CHECK, BET_OR_CALL, RAISE};
    enum enabled_actions action;
    

	int playersLeft()
	{
		int count = 0;
		for (int i = 0; i < players_count; i++)
			if (players[i].money > 0)
				count++;
		return count;
	}

	int computerAction(int playerNum)
	{
		if (players[playerNum].cards[0].rank < 8 && players[playerNum].cards[1].rank < 8)
		{
			if (players[playerNum].cards[0].rank != players[playerNum].cards[1].rank)
				return 0;
			else
				return 1;
		}

		else if (players[playerNum].cards[0].rank < 10 && players[playerNum].cards[1].rank < 10)
		{
			if (players[playerNum].cards[0].rank != players[playerNum].cards[1].rank)
				return 1;
			else
				return 2;
		}
		else
		{
			return 2;
		}
	}

	/*checks if someone still got bet/call*/
	bool playersToBet()
	{
		for (int i = 0; i < players_count; i++)
			if (players[i].round == 1 && players[i].goodToGo == 0)
				return true;

		return false;
	}

	void takeBets()
	{
		using std::cout;
		using std::cin;
		using std::endl;

		betOn = 0;
		for (int k = 0; k < players_count; k++)
			players[k].goodToGo = 0;

		for (int k = bind + 1; k < bind + 1 + players_count; k++)
		{
			/* human player actions */
			if (k % players_count == 4 && players[player_index].round)
			{
				
				if (betOn)
				{
					cout << "\t\t\t\t\tYour action: (1) FLOP (3) BET/CALL (4) RAISE ";
					cin >> picked_action;
					read(players[player_index].player_socket,players[player_index].player_input,1);
					picked_action = players[player_index].player_input[0];
					cout << picked_action;
                    action = (enabled_actions)picked_action;
					while (action != FLOP && action != BET_OR_CALL && action != RAISE)
					{
						cout << "Invalid number pressed." << endl;
						cout << "\t\t\t\t\tYour action: (1) FLOP (3) BET/CALL ";
					    cin >> picked_action;
                        action = (enabled_actions)picked_action;
					}
					if (picked_action == RAISE)
                    {
						if (players[player_index].money <= betOn){
						while (action != FLOP && action != BET_OR_CALL)
					    {
							cout << "You don't have enough money to RAISE!"<<endl;
							cout << "Invalid number pressed." << endl;
						    cout << "\t\t\t\t\tYour action: (1) FLOP (3) BET/CALL (4) RAISE";
						    cin >> picked_action;
                            action = (enabled_actions)picked_action;
					    }
						}
					}
				}
				else
				{
					cout << "\t\t\t\t\tYour action: (1) FLOP (2) CHECK (3) BET/CALL (4) RAISE ";
					cin >> picked_action;
                    action = (enabled_actions)picked_action;
                    if (players[player_index].money == 0 && picked_action == 3)
                    {
                        while (picked_action < 1 || picked_action > 2)
					    {
                            if (picked_action == BET_OR_CALL) {cout << "You have no money to make a BET/CALL"<<endl;}
							cout << "Invalid number pressed." << endl;
						    cout << "\t\t\t\t\tYour action: (1) FLOP (2) CHECK (3) BET/CALL (4) RAISE";
						    cin >> picked_action;
                            action = (enabled_actions)picked_action;
					    }
                    }
					else if (picked_action == RAISE)
                    {
						if (players[player_index].money <= betOn || betOn == 0){
						while (picked_action < 1 || picked_action > 3)
					    {
							cout << "You don't have enough money to RAISE!"<<endl;
							cout << "Invalid number pressed." << endl;
						    cout << "\t\t\t\t\tYour action: (1) FLOP (2) CHECK (3) BET/CALL (4) RAISE";
						    cin >> picked_action;
                            action = (enabled_actions)picked_action;
					    }
						}
					}
					else 
                    {
                        while (picked_action < 1 || picked_action > 4)
					    {
						    cout << "Invalid number pressed." << endl;
						    cout << "\t\t\t\t\tYour action: (1) FLOP (2) CHECK (3) BET/CALL (4) RAISE";
						    cin >> picked_action;
                            action = (enabled_actions)picked_action;
					    }
                    }
				}

				cout << endl;

				if (action == FLOP)
				{
					players[player_index].round = 0;
					cout << "\t- " << players[player_index].name << " flops...\n";
				}
				else if (action == CHECK)
				{
					cout << "\t+ " << players[player_index].name << " checks.\n";
					continue;
				}
				else if (action == BET_OR_CALL)
				{
					if (betOn)
					{
                        if (betOn > players[player_index].money)
                        {
                            cout << "\t\tYou don't have that amount of money!"<<endl;
                            cout << "\t\tDo You want to go ALL IN!?" << endl;
                            cout << "\t\tYour action: (1) Flow (2) ALL IN"<<endl;
                            cin >> picked_action;
                            action = (enabled_actions)picked_action;
                            if (action == FLOP)
				            {
					            players[player_index].round = 0;
					            cout << "\t- " << players[player_index].name << " flops...\n";
				            }
                            else
                            {
                                pot += players[player_index].money;
                                players[player_index].money = 0;
                                players[player_index].goodToGo = 1;
                                players[player_index].round = 0;
                                players[player_index].IfAllIn = true;                             
                                cout << "\t+ " << players[player_index].name << " GOES ALL IN! " << "$\n";
                            }
                        }
                        else
                        {
						    pot += betOn;
						    players[player_index].money -= betOn;
						    players[player_index].goodToGo = 1;
						    cout << "\t+ " << players[player_index].name << " bets " << betOn << "$\n";
                        }
					}
					else
					{
                        cout << "How much do you want to bet: ";
						cin >> bet;
						
                        while (bet > players[player_index].money || bet < 1)
						{
							cout << "Invalid number to bet." << endl;
							cout << "How much do you want to bet: ";
							cin >> bet;						
							cout << endl << endl;
						}
						pot += bet;
						players[player_index].money -= bet;
						betOn = bet;
						players[player_index].goodToGo = 1;

						cout << "\t+ " << players[player_index].name << " bets " << bet << "$\n";
					}
				}
				else //RAISE option
				{
						cout << "How much do you want to RAISE: ";
						cin >> bet;
						while (bet > betOn + players[player_index].money)
						{
							cout << "Invalid number to RAISE." << endl;
							cout << "How much do you want to RAISE: ";
							cin >> bet;						
							cout << endl << endl;
						}
						pot += bet + betOn;
						players[player_index].money -= (bet + betOn);
						betOn += bet;
						players[player_index].goodToGo = 1;

						cout << "\t+ " << players[player_index].name << " raises " << bet << "$\n";
						for (int p = 0; p < players_count; p++)
						{
							if (p != player_index)
							{
								players[p].round = 1;
								players[p].goodToGo = 0;
							}						
						}					
				}
			}
			/* computers actions */
			else
			{
				if (players[k % players_count].round == 0)
				{
					continue;
				}
				rational = (rand() % 2) + 1;
				if (rational > 1)
				{
					action = (enabled_actions)computerAction(k % players_count);
				}
				else
				{
					action = (enabled_actions)((rand() % 3) + 1);
				}
				if (action == FLOP)
				{
					players[k % players_count].round = 0;
					cout << "\t- " << players[k % players_count].name << " flops..." << endl;
				}
				else if (action == CHECK && betOn == 0)
				{
                
					cout << "\t+ " << players[k % players_count].name << " checks." << endl;
					continue;
				}
				else
				{
					if (betOn)
					{
                        if (betOn > players[k % players_count].money)
                        {
                            action = (enabled_actions)((rand() % 2) + 1);
                            if (action == FLOP)
				            {
					            players[k % players_count].round = 0;
					            cout << "\t- " << players[k % players_count].name << " flops...\n";
				            }
                            else
                            {
                                pot += players[k % players_count].money;
                                players[k % players_count].money = 0;
                                players[k % players_count].goodToGo = 1;
                                players[k % players_count].round = 0;
                                players[k % players_count].IfAllIn = true;

                                cout << "\t+ " << players[k % players_count].name << " GOES ALL IN! " << "$\n";
                            }
                        }
                        else
                        {
							if ((rand()%4) !=0)
							{
						    	pot += betOn;
						    	players[k % players_count].money -= betOn;
						    	players[k % players_count].goodToGo = 1;
						    	cout << "\t+ " << players[k % players_count].name << " bets " << betOn << "$\n";
							}
							else //RAISE option
							{
							while(bet > 200)
							{
							bet = rand()%(players[k % players_count].money - betOn);
							}
							pot += bet + betOn;
							players[k % players_count].money -= (bet + betOn);
							betOn += bet;
							players[k % players_count].goodToGo = 1;

							cout << "\t+ " << players[k % players_count].name << " raises " << bet << "$\n";
							for (int p = 0; p < players_count; p++)
							{
								if (p != (k % players_count))
								{
								players[p].round = 1;
								players[p].goodToGo = 0;
								}						
							}					
				//end of RAISE
							}
						}
					}
					else
					{
						
						bet = (rand() % (players[k % players_count].money / 3) + 10);
						while(bet > players[k % players_count].money) {bet = (rand() % (players[k % players_count].money / 3) + 10);}
						pot += bet;
						players[k % players_count].money -= bet;
						cout << '\a';
						cout << "\t+ " << players[k % players_count].name << " bets " << bet << "$" << endl;
						betOn = bet;
						players[k % players_count].goodToGo = 1;
					}
				}
				_sleep(1);
			}
		}

		if (betOn && playersToBet())
		{
			for (int k = bind + 1; k < bind + 7; k++)
			{
				if (k % players_count == 4)
				{
					if (players[player_index].round && players[player_index].goodToGo == 0)
					{
						cout << "\t\t\t\t\tYour action: (1) FLOP (3) BET/CALL (4) RAISE ";
						cin >> picked_action;
                        action = (enabled_actions)picked_action;
						while (action != FLOP && action != BET_OR_CALL && action != RAISE)
						{
							cout << "Invalid number pressed." << endl;
							cout << "\t\t\t\t\tYour action: (1) FLOP (3) BET/CALL ";
							cin >> picked_action;
                            action = (enabled_actions)picked_action;
							cout << endl << endl;
						}
						
						if (picked_action == RAISE)
                    	{
							if (picked_action == RAISE) {cout << "There is no bet to RAISE! " << endl;}
							if (players[player_index].money <= betOn) cout << "You don't have enough money to RAISE!"<<endl;
							while (action != FLOP && action != BET_OR_CALL)
					    	{
								cout << "Invalid number pressed." << endl;
						    	cout << "\t\t\t\t\tYour action: (1) FLOP (3) BET/CALL ";
						    	cin >> picked_action;
                            	action = (enabled_actions)picked_action;
					    	}
						}
						if (action == FLOP)
						{
							cout << "\t- " << players[player_index].name << " flops...\n";
							players[player_index].round = 0;
						}
						else if (action == BET_OR_CALL && betOn > players[player_index].money)
                        {
                            cout << "\t\tYou don't have that amount of money!"<<endl;
                            cout << "\t\tDo You want to go ALL IN!?" << endl;
                            cout << "\t\tYour action: (1) Flow (2) ALL IN"<<endl;
                            cin >> picked_action;
                            action = (enabled_actions)picked_action;
                            if (action == FLOP)
				            {
					            players[player_index].round = 0;
					            cout << "\t- " << players[player_index].name << " flops...\n";
				            }
                            else
                            {
                                pot += players[player_index].money;
                                players[player_index].money = 0;
                                players[player_index].goodToGo = 1;
                                players[player_index].round = 0;
                                players[player_index].IfAllIn = true;

                                
                                cout << "\t+ " << players[player_index].name << " GOES ALL IN! " << "$\n";
                            }
                        }
						else if (action == RAISE) //RAISE option
						{
						cout << "How much do you want to RAISE: ";
						cin >> bet;
						while (bet > betOn + players[player_index].money)
						{
							cout << "Invalid number to RAISE." << endl;
							cout << "How much do you want to RAISE: ";
							cin >> bet;						
							cout << endl << endl;
						}
						pot += bet + betOn;
						players[player_index].money -= (bet + betOn);
						betOn += bet;
						players[player_index].goodToGo = 1;

						cout << "\t+ " << players[player_index].name << " raises " << bet << "$\n";
						for (int p = 0; p < players_count; p++)
						{
							if (p != player_index)
							{
								players[p].round = 1;
								players[p].goodToGo = 0;
							}						
						}						
						}
						else
						{
							pot += betOn;
							players[player_index].money -= betOn;
							players[player_index].goodToGo = 1;

							cout << "\t+ " << players[player_index].name << " bets " << betOn << "$\n";
						}
					}
				}

				else
				{
					if (players[k % players_count].round == 0 || players[k % players_count].goodToGo == 1)
						continue;
					action = (enabled_actions)((rand() % 2)+1);
					if (action == FLOP)
					{
						players[k % players_count].round = 0;
						cout << "\t- " << players[k % players_count].name << " flops..." << endl;
					}
					else
					{
					    if (betOn > players[k % players_count].money)
                        {
                            action = (enabled_actions)((rand() % 2) + 1);
                            if (action == FLOP)
				            {
					            players[k % players_count].round = 0;
					            cout << "\t- " << players[k % players_count].name << " flops...\n";
				            }
                            else
                            {
                                pot += players[k % players_count].money;
                                players[k % players_count].money = 0;
                                players[k % players_count].goodToGo = 1;
                                players[k % players_count].round = 0;
                                players[k % players_count].IfAllIn = true;

                                cout << "\t+ " << players[k % players_count].name << " GOES ALL IN! " << "$\n";
                            }
                        }
						else
						{
						pot += betOn;
						players[k % players_count].money -= betOn;
						cout << "\t++ " << players[k % players_count].name << " calls!" << endl;
						players[k % players_count].goodToGo = 1;
						}
					}
				}
			}
		}
	}

	bool oneLeft()
	{
		int count = 0;
		for (int k = 0; k < players_count; k++)
			if (players[k].round)
				count++;
		if (count == 1)
			return true;
		else
			return false;
	}

	int getWinner()
	{
		int winner;
		for (int k = 0; k < players_count; k++)
			if (players[k].round)
				winner = k;
		return winner;
	}

	int getScore(Card hand[])
	{
		qsort(hand, 5, sizeof(Card), compareCards);
		int straight, flush, three, four, full, pairs, high;
		int k;

		straight = flush = three = four = full = pairs = high = 0;
		k = 0;

		/*checks for flush*/
		while (k < 4 && hand[k].suit == hand[k + 1].suit)
			k++;
		if (k == 4)
			flush = 1;

		/* checks for straight*/
		k = 0;
		while (k < 4 && hand[k].rank == hand[k + 1].rank - 1)
			k++;
		if (k == 4)
			straight = 1;

		/* checks for fours */
		for (int i = 0; i < 2; i++)
		{
			k = i;
			while (k < i + 3 && hand[k].rank == hand[k + 1].rank)
				k++;
			if (k == i + 3)
			{
				four = 1;
				high = hand[i].rank;
			}
		}

		/*checks for threes and fullhouse*/
		if (!four)
		{
			for (int i = 0; i < 3; i++)
			{
				k = i;
				while (k < i + 2 && hand[k].rank == hand[k + 1].rank)
					k++;
				if (k == i + 2)
				{
					three = 1;
					high = hand[i].rank;
					if (i == 0)
					{
						if (hand[3].rank == hand[4].rank)
							full = 1;
					}
					else if (i == 1)
					{
						if (hand[0].rank == hand[4].rank)
							full = 1;
					}
					else
					{
						if (hand[0].rank == hand[1].rank)
							full = 1;
					}
				}
			}
		}

		if (straight && flush)
			return 170 + hand[4].rank;
		else if (four)
			return 150 + high;
		else if (full)
			return 130 + high;
		else if (flush)
			return 110;
		else if (straight)
			return 90 + hand[4].rank;
		else if (three)
			return 70 + high;

		/* checks for pairs*/
		for (k = 0; k < 4; k++)
			if (hand[k].rank == hand[k + 1].rank)
			{
				pairs++;
				if (hand[k].rank>high)
					high = hand[k].rank;
			}

		if (pairs == 2)
			return 50 + high;
		else if (pairs)
			return 30 + high;
		else
			return hand[4].rank;
	}

	int tryHand(int array[], int player)
	{
		Card hand[5];

		/* get cards from table and player */
		for (int i = 1; i < 4; i++)
			hand[i - 1] = tableCards[array[i]];

		for (int i = 0; i < 2; i++)
			hand[i + 3] = players[player].cards[i];

		return getScore(hand);

	}

	void evaluateHands()
	{
		int stack[10], k;
		int currentPoints;

		for (int q = 0; q < players_count; q++)
		{
			if (players[q].round)
			{
				stack[0] = -1; /* -1 is not considered as part of the set */
				k = 0;
				while (1) {
					if (stack[k] < 4)
					{
						stack[k + 1] = stack[k] + 1;
						k++;
					}

					else
					{
						stack[k - 1]++;
						k--;
					}

					if (k == 0)
						break;

					if (k == 3)
					{
						currentPoints = tryHand(stack, q);
						if (currentPoints > handPoints[q])
						{
							handPoints[q] = currentPoints;
							for (int x = 0; x < 3; x++)
								bestHand[q][x] = stack[x + 1];
						}
					}
				}

			}
		}
	} /*end of evaluateHands() */

	void printWinningHand(int winner)
	{
		using std::cout;
		using std::endl;
	if (winner != -1)
	{
		Card winningHand[5];
		for (int i = 0; i < 3; i++)
			winningHand[i] = tableCards[bestHand[winner][i]];

		for (int i = 0; i < 2; i++)
			winningHand[i + 3] = players[winner].cards[i];

		qsort(winningHand, 5, sizeof(Card), compareCards);

		cout << "   ___   ___   ___   ___   ___" << endl;
		cout << "  | " << ranks[winningHand[0].rank] << " | | " << ranks[winningHand[1].rank] << " | | " << ranks[winningHand[2].rank] << " | | " << ranks[winningHand[3].rank] << " | | " << ranks[winningHand[4].rank] << " |" << endl;
		cout << "  | " << suits[winningHand[0].suit] << " | | " << suits[winningHand[1].suit] << " | | " << suits[winningHand[2].suit] << " | | " << suits[winningHand[3].suit] << " | | " << suits[winningHand[4].suit] << " |" << endl;
		cout << "  |___| |___| |___| |___| |___|" << endl;
		cout << endl << endl;
		_sleep(3);
	}
	}

	void FindWinner()
	{
					/* find and declare round winner */
			maxPoints = 0;
			roundWinner = -1;
			for (int q = 0; q < players_count; q++)
			{
				if (players[q].round || players[q].IfAllIn == true)
				{
					if (handPoints[q] > maxPoints)
					{
						maxPoints = handPoints[q];
						roundWinner = q;
					}
				}
			}
			std::cout << std::endl;
			std::cout << players[roundWinner].name << " wins $" << pot << " with ";
			if (maxPoints < 30)
				std::cout << "HIGH CARD";
			else if (maxPoints < 50)
				std::cout << "SINGLE PAIR";
			else if (maxPoints < 70)
				std::cout << "TWO PAIRS";
			else if (maxPoints < 90)
				std::cout << "THREE OF A KIND";
			else if (maxPoints < 110)
				std::cout << "STRAIGHT";
			else if (maxPoints < 130)
				std::cout << "FLUSH";
			else if (maxPoints < 150)
				std::cout << "FULL HOUSE";
			else if (maxPoints < 170)
				std::cout << "FOUR OF A KIND";
			else
				std::cout << "STRAIGHT FLUSH";
			std::cout << "\n\n";

            std::cout << "\tThe winning hand:"<<std::endl;
			printWinningHand(roundWinner);
	}

	/* main gameplay function*/
	void startGame()
	{
		int i = 0;

		while (playersLeft() > 1)
		{
			/* starting default values*/
			for (int z = 0; z < players_count; z++)
				if (players[z].money<1)
				{
					players[z].playing = 0;
					players[z].round = 0;
				}
			for (int z = 0; z < players_count; z++)
			{
				if (players[z].playing)
					players[z].round = 1;
				handPoints[z] = -1;
			}
			for (int x = 0; x < players_count; x++)
			{
				for (int y = 0; y<3; y++)
				{
					bestHand[x][y] = -1;
				}
			}

			/* checking for game over*/
			if (players[player_index].playing == 0)
			{
				std::cout << "You are out of money, sorry." << std::endl;
				std::cout << "Game over." << std::endl;
				break;
			}

			bind = i % players_count;

			/* paying bind */
			pot = 20;
			if (players[bind].money >= 20)
				players[bind].money -= 20;
			else
				players[bind].playing = 0;

			std::cout << "\n\n\n\n\n\n\n";
			std::cout << "\t\t\t\t\t ------ ROUND " << i + 1 << " ------\n\n\n";
			_sleep(sleep_time);
			deck1.shuffle();

			/* pre-flop */
			deal();
			_sleep(sleep_time);
			printTable();
			takeBets();
			if (oneLeft())
			{
				flop();
				river();
				FindWinner();
				winner = getWinner();
				std::cout << players[winner].name << " wins $" << pot << "\n\n";
                printWinningHand(winner);
            for(int l = 0; l < players_count; l++)
            {
                if ((players[l].round == true || players[l].IfAllIn == true) && l != winner ) 
                {
                std::cout << "\t" << players[l].name << std::endl;
                printWinningHand(l);
                players[l].IfAllIn = false;        
                }
            }
                players[winner].money += pot;
				i++;
				continue;
			}

			/* flop */
			flop();
			std::cout << std::endl;
			_sleep(sleep_time);
			printTable();
			takeBets();
			if (oneLeft())
			{
				turn();
				river();				

				FindWinner();
				winner = getWinner();
				std::cout << players[winner].name << " wins $" << pot << "\n\n";
                printWinningHand(winner);
            for(int l = 0; l < players_count; l++)
            {
                if ((players[l].round == true || players[l].IfAllIn == true) && l != winner ) 
                {
                std::cout << "\t" << players[l].name << std::endl;
                printWinningHand(l);
                players[l].IfAllIn = false;
                
                }
            }
                players[winner].money += pot;
				i++;
				continue;
			}

			/* turn */
			turn();
			std::cout << std::endl;
			_sleep(sleep_time);
			printTable();
			takeBets();
			if (oneLeft())
			{
				
				river();
				FindWinner();
				winner = getWinner();
				std::cout << players[winner].name << " wins $" << pot << "\n\n";
                printWinningHand(winner);
                for(int l = 0; l < players_count; l++)
                {
                if ((players[l].round == true || players[l].IfAllIn == true) && l != winner ) 
                {
                std::cout << "\t" << players[l].name << std::endl;
                printWinningHand(l);
                players[l].IfAllIn = false;
                }
            	}
                players[winner].money += pot;
				i++;
				continue;
			}

			/* river */
			river();
			std::cout << std::endl;
			_sleep(sleep_time);
			printTable();
			takeBets();
			evaluateHands();
			FindWinner();
            if (playersLeft() > 1) std::cout << std::endl << "Hands of ramaining players:" << std::endl;

            for(int l = 0; l < players_count; l++)
            {
                if ((players[l].round == true || players[l].IfAllIn == true) && l != roundWinner ) 
                {
                std::cout << "\t" << players[l].name << std::endl;
                printWinningHand(l);
                players[l].IfAllIn = false;
                
                }
            }
			players[roundWinner].money += pot;
			i++;
		}

	}
};
char* WelcomeScreen(char* nejm)
{

	srand(time(NULL));

	using std::cout;
	using std::endl;

	cout << "Welcome to..." << endl << endl;
	cout << "#######                        ###### " << endl;
	cout << "   #    ###### #    # #####    #     #  ####  #    # ###### #####" << endl;
	cout << "   #    #       #  #    #      #     # #    # #   #  #      #    #" << endl;
	cout << "   #    #####    ##     #      ######  #    # ####   #####  #    #" << endl;
	cout << "   #    #        ##     #      #       #    # #  #   #      #####" << endl;
	cout << "   #    #       #  #    #      #       #    # #   #  #      #   #" << endl;
	cout << "   #    ###### #    #   #      #        ####  #    # ###### #    #" << endl << endl;


	cout << "OK " << nejm << " let's play some poker!" << endl << endl;

}

int main()
{
	PokerGame game1;


    ssize_t n;
    struct epoll_event event, events[10];
    int epoll_fd = epoll_create1(0);
    if(epoll_fd == -1)
    {
        printf("Failed to create epoll file descriptor!");
        return 1;
    }
    char str[100], answer[20] = "Message received!",conn_message[100] = "Witaj ",name[100];
    int listen_fd, comm_fd, running =1, event_count, i,sock_fd,temp,cliCounter=0, k;
    char arrOfFd[10][100];
    int ifUser[100] = {0};
    struct sockaddr_in servaddr;
    struct sockaddr_in clientaddr;
    socklen_t clilen;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_fd == -1)
    {
        printf("Failed to create socket!");
        return 1;
    }  

    temp=fcntl(listen_fd, F_GETFL);
    temp = temp | O_NONBLOCK;
    fcntl(listen_fd,F_SETFL,temp);

    
    event.events = EPOLLIN;
    event.data.fd = listen_fd;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event))
    {
        printf("Failed to add file descriptor to epoll\n");
        close(epoll_fd);
        return 1;
    }

    bzero( &servaddr, sizeof(servaddr));
 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(22000);
 
    if(bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0)
    {
        printf("Failed to bind!");
        return 1;
    }
 
    listen(listen_fd, 10);
 /////

    printf("Waiting for clients to connect...\n");
    while(running)
    {   
        event_count = epoll_wait(epoll_fd, events, 10, 30000);
        for(i = 0; i < event_count; i++)
        {
            if(events[i].data.fd == listen_fd)
            {
                comm_fd = accept(listen_fd, (struct sockaddr*) &clientaddr, &clilen);
                if(comm_fd < 0)
                {
                    printf("Failed to accept connection!");
                }
                else
                {  
                    read(comm_fd,name,10);
                    printf("Connection from %s accepted! fd(%d)\n", name, comm_fd);
                    cliCounter++;
                    ifUser[comm_fd] = 1;
					strcpy(arrOfFd[comm_fd], name);
                    
                    if(cliCounter > 0)
                    {
                        printf("\nConnected clients: ");
                        for(k = 0; k <100;k++)
                        {
                        if(ifUser[k] == 1) {printf(" %s",arrOfFd[k]);}
                        }
                        printf("\n");
                    }
                    printf("Waiting for input...\n");
                }
                ///////////
                temp=fcntl(listen_fd, F_GETFL);
                 temp = temp | O_NONBLOCK;
                fcntl(listen_fd,F_SETFL,temp);

                event.data.fd = comm_fd;
                event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epoll_fd,EPOLL_CTL_ADD,comm_fd,&event);

            }
            else if (events[i].events & EPOLLIN)
            {
                if((sock_fd = events[i].data.fd) < 0) continue;
                if((n= read(sock_fd, str, 100))<0)
                {
                    if(errno == ECONNRESET)
                    {
                        close(sock_fd);
                        events[i].data.fd = -1;
                    }
                    else
                    {
                        printf("Readline error");
                    }
                }
                else if (n ==0 )
                {
                    close(sock_fd);
                    events[i].data.fd = -1;
                }
                printf("Received message from %s: %s\n",arrOfFd[events[i].data.fd],str);
                if(!strcmp("Stop",str)){
                    cliCounter--;
                    ifUser[events[i].data.fd] = 0;
                    printf("%s left session\n", arrOfFd[events[i].data.fd]);
                    close(events[i].data.fd);
                }
                bzero(str,100);
                event.data.fd=sock_fd;
                event.events=EPOLLOUT | EPOLLET;
                epoll_ctl(epoll_fd, EPOLL_CTL_MOD,sock_fd,&event);
            }
            else if (events[i].events & EPOLLOUT)
            {
                sock_fd = events[i].data.fd;
                dup2(comm_fd,1);
				WelcomeScreen(name);
				game1.start(name);
                event.data.fd=sock_fd;
                event.events= EPOLLIN | EPOLLET;
                epoll_ctl(epoll_fd, EPOLL_CTL_MOD,sock_fd,&event);
                if(cliCounter > 0)
                {
                    printf("\nConnected clients: ");
                    for(k = 0; k <100;k++)
                    {
                    if(ifUser[k] == 1) {printf(" %s",arrOfFd[k]);}
                    }
                    printf("\n");
                }
                printf("Waiting for input...\n");
            }      
        }
   }
if(close(epoll_fd))
{
    printf("Failed to close epoll file descriptor\n");
    return 1;
}
return 0;
}