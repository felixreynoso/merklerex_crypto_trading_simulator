#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"


class MerkelMain
{
    public:
        MerkelMain();
        /** Call this to start the sim */
        void init();
    private: 
        void printMenu();
        void printHelp(std::string cmd = "all");
        void printMarketStats();
        void enterAsk(std::vector<std::string> params);
        void enterBid(std::vector<std::string> params);
        void printWallet();
        void printCurrentTimeStep();
        void printknownProducts();
        void gotoNextTimeframe();
        std::vector<double> computeAvgOrderOverNumberOfSteps(std::string product, OrderBookType orderType, int numberOfSteps = 0);
        void printMinimum(std::string product, OrderBookType orderType);
        void printMaximum(std::string product, OrderBookType orderType);
        void processPrintMinOrderInput(std::vector<std::string> params);
        void processPrintMaxOrderInput(std::vector<std::string> params);
        void processAvgOrdersInput(std::vector<std::string> params);
        void processPredictOrdersInput(std::vector<std::string> params);
        void predictProductFigures(std::string bound,std::string product, OrderBookType orderType);
        
        void getUserInput();

        std::string currentTime;

        OrderBook orderBook{"20200317.csv"};

        Wallet wallet;

};
