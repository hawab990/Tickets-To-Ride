/* Program name: 	    Tickets To Ride
Project file name:
Author:		    Sakinah and AbdelRahman
Date:			17/11/2015
Language:		    C++
Platform:		    Microsoft Visual Studio 2013
Purpose:		    This game simulates the physical version of the Tickets to ride board game.
Description:		This simulates the known Tickets to ride boardgame where the person who captures the most  roots while fulfulling their missions wins the game with the most points.
*/
#include "RandomSolution.h"

#include<iostream>

using namespace std;

RandomSolution::RandomSolution(char *user, char *pass, 
			char *agent, char *host, int port) 
	: TTRClient(user, pass, agent, host, port)
{
	myRand = new TTRGenerator();
}
 
void RandomSolution::myMethod(void)
{
	// Set action performed flag to false as we have not acted this turn.
	actionPerformed = false;
	// If (firstMissions==true) reject last received mission.
	if (firstMissions) discardHighestMission();
	else 	
		// As long as we have not performed an action, attempt to choose an action to perform.
		while ((!actionPerformed)&&(play)&&(myTurn))
		{
			 // Randomly select an action.
			//int action = myRand->get()%5;
			
			// display hand
			TTRCard **myCards = myPlayer->getCards();
			// Search for cards that match the edge's color.
			for (int i = 0; i<nColors; i++)
			{
				
				TTRCard *card = myCards[i];
				cerr << card->getColor() << ":" << card->getCount() << " ";
			}
			cerr << endl;
			attemptToClaimRoute();
			
			/*cerr<<endl;*/
			// Let's see what we chose:
			//switch (action) 
			//{
			//	// Attempt to draw a card from the open deck.
			//	case DRAW_FROM_OPEN_DECK: attemptToDrawFromDeck(); break;
			//	
			//	// Attempt to draw a card from the blind deck.
			//	case DRAW_FROM_DECK: attemptToDrawFromDeck(); break;
			//	
			//	// Attempt to draw missions.
			//	case DRAW_MISSIONS: attemptToDrawMissions(); break;
			//	
			//	// Attempt to claim a route.
			//	case CLAIM_ROUTE: attemptToClaimRoute(); break;
			//	
			//	// Attempt to build a station.
			//	case BUILD_STATION: attemptToBuildStation(); break;
			//	
			//	// We have no idea what we tried to do; better try again. 
			//	default: break;
			//}
		}
	}

void RandomSolution::discardLastMission(){
	int ret = 0;
	ArrayList<TTRMission> *m = myPlayer->getMissionArray();
	while ((ret != 1)&&(play)){
		ret = rejectMissions(m->get(m->size()-1), (TTRMission *)0);
		actionPerformed = true;
	}
}


void RandomSolution::discardHighestMission(){
	int ret = 0;
	ArrayList<TTRMission> *m = myPlayer->getMissionArray();

	int pos1 = 0;
	int pos2 = 0;

	int max = -1;
	for(int i = 0; i < m->size(); i++)
	{
		if(max < m->get(i)->getValue())
		{
			max = m->get(i)->getValue();
			pos1 = i;
		}
	}

	max = -1;
	for(int i = 0; i < m->size(); i++)
	{
		if(i == pos1)
			continue;
		if(max < m->get(i)->getValue())
		{
			max = m->get(i)->getValue();
			pos2 = i;
		}
	}

	while ((ret < 1)&&(play)){
		ret = rejectMissions(m->get(pos1), m->get(pos2));
		actionPerformed = true;
	}
}


void RandomSolution::attemptToDrawFromDeck(){
	// Draw the card.
	int ret = drawCardFromDeck();
	if ((ret==1)||(ret==0))
	{
		// If successful, try to draw another card from the blind deck.
		int ret2 = 0;
		// Keep trying until successful (as long as we are still playing).
		while ((ret2 != 1)&&(play))
			ret2 = drawCardFromDeck();
		// Set actionPerformed flag to true.
		actionPerformed = true;
	}
}
	
void RandomSolution::attemptToDrawMissions(){
	// Attempt to draw missions.
	int ret = drawMissions();
	if (ret>0)
	{
		// If successful
		ret = 0;
		// While not successful attempt to keep all missions.
		while ((ret != 0)&&(play))
			ret = keepAllMissions();
	}
	// Set actionPerformed flag to true.
	actionPerformed = true;
}
	// Finding the longest  vacant route.
void RandomSolution::attemptToClaimRoute(){
	// Initialize edge to claim.
	TTREdge *edge = (TTREdge *)0;// Same as doing a null pointer.
	// Get number of nodes on map.
	int n = myMap->getNAdj();// Returns the number of cities there are on the map.
	// Initialize source and destination node ids.


	TTREdge *longest = (TTREdge *)0;// initializing it to be nothing.
	int max = 0;
	// As long as the edge we chose is null, randomly pick a different edge. And finding longest route
	for (int i = 0; i < myMap->getNAdj(); i++) { //number of edges for the entire board
		for (int j = 0; j < myMap->getNAdj(); j++) {
			//If it does not have an owner, and doesn't overpass maximum number of nodes, search through the map
			//While edge is not initialized  or the edge under question is owwned by a player, its only going to check  untill the maximum number number of nodes are reaached.
			
				//for (int i = 0; i < myPlayer->getMissionArray()->size(); i++) {
				//s = myPlayer->getMissionArray()->get(i)->getNodeA()->getId();
				//d = myPlayer->getMissionArray()->get(i)->getNodeB()->getId();
				//}
				//s = myRand->get() % n;
				//d = myRand->get() % n;

				cerr << i << " " << j << endl;

				edge = myMap->getAdj()[i][j];// Grabs the i-start and J- end and returns the route between them.
				if (edge != (TTREdge *)0 && isOwned(edge) == false) {//if thhe edge variable has something thing in it, then get the number of cards to complete that route.
					cerr << "Inside edge " << edge->getCars() << endl;
					int temp = edge->getCars();
					if (max < temp) {// if the currently tracked max is less then the new edges length, then swap it with the maximun essentially making that new edge the longest one.
						max = temp;
						longest = edge;
					}
					else {
						edge = (TTREdge*)0;
					}
				}
		}
	}

	// Get edge color.
	char color = longest->getColor(0);
	// Get out player's cards in hand.
	TTRCard **myCards = myPlayer->getCards();
	// Get number of cars and engines required to claim route.
	int cars = longest->getCars(), engs = longest->getEngines();
	// Search for cards that match the edge's color.
	for (int i=0; i<nColors-1; i++)
	{
		if ((myCards[i]->getColor() == color) || (color == '*'))
		{
			TTRCard *card = myCards[i];
			if (card->getCount() < cars)
			{
				// Figure out how to pay for edge.
				//action = DRAW_FROM_DECK;
				attemptToDrawFromDeck();
				//engs = engs + cars - card->getCount();
				//cars = card->getCount();
			}
			else {
				// Attempt to claim edge.
				cerr << "Attempt " << color << endl;
				int ret = claimRoute(longest->getNodeA()->getId(), longest->getNodeB()->getId(), myCards[i]->getColor(), cars, engs);

				// If successful, set actionPerformed flag to true
				if (ret >= 0) {
					// If more cards are required
					if (ret>0)
						// While not successful attempt to pass.
						while ((ret != 0) && (play))
							ret = claimPass();
					actionPerformed = true;
				}
			}
			break;
		}
	}
}
//	
//void RandomSolution::attemptToBuildStation(){
//	// If we have no stations left, move on
//	if (myPlayer->getNStations() == 0) return;
//	// Initialize edge to build station on.
//	TTREdge *edge = (TTREdge *)0; 
//	// Get number of nodes on map.
//	int n = myMap->getNAdj();
//	// Initialize source and destination nodes.
//	int s=0, d=0;
//	// Randomly pick an edge.
//	while (edge == (TTREdge *)0)
//	{
//		s = myRand->get()%n; 
//		d = myRand->get()%n;
//		edge = myMap->getAdj()[s][d];
//	}
//	// Get number of required cards to build station.
//	int req = 4 - myPlayer->getNStations();
//	char color = '0'; 
//	// Get first card in card array to have enough cards to build station.
//	TTRCard **myCards = myPlayer->getCards();
//	for (int i=0; i<nColors-1; i++)
//	{
//		if (myCards[i]->getCount() >= req)
//		{
//			color = myCards[i]->getColor();
//			break;
//		}
//	}
//	// Attempt to build station.
//	int ret = buildStation(s, d, color, req, 0);
//	// If successful, set actionPerformed flag to true.
//	if (ret==1) actionPerformed = true;
//}

bool RandomSolution::isOwned(TTREdge *edge) {
	if (edge != (TTREdge*)0) {
	for (int i = 0; i < 5; i++) {
		if (edge->getOwner(i) != (TTRPlayer*)0) {
			return true;
		}
	}
	return false;
	}
	return false;
}

RandomSolution::~RandomSolution(void){}
