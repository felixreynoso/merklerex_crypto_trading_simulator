#include "MerkelMain.h"
#include <iostream>
#include <vector>
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <numeric>
#include <cstdlib>
#include <unistd.h>
#include <cctype>
#include <algorithm>
#include <sstream>

using namespace std;

MerkelMain::MerkelMain()
{
}

void MerkelMain::init()
{
    int input;

    currentTime = orderBook.getEarliestTime();
    wallet.insertCurrency("BTC", 10);
    usleep(1500000);
    system("clear");

    cout << "Welcome to Merklerex v0.1, a Cryptocurrency Exchange Simulator." << endl;
    cout << "Type \".help\" for more information." << endl;
    
    while(true)
    {
        cout << "> ";  
        try 
        {
            getUserInput();
        }
        catch (string message) 
        {
            cout << message;
        }

    }
}

void MerkelMain::getUserInput()
{
    string input;
    // get command name and parameters from user
    getline(cin, input);

    // Uses the tokenise function to parse space separated parameters and store them in 'params'
    vector<string> params = CSVReader::tokenise(input, ' ');

    // Check that user typed something
    if (params.size() > 0) {
        string cmd = params[0];
        // Convert the first paramater captured by tokenise to lowercase to normalize formatting
        for_each(cmd.begin(), cmd.end(), [](char & c) {c = ::tolower(c);});

        if (cmd == "help")
        {
            if (params.size() >= 2)
            {
                string helpCmd = params[1];
                for_each(helpCmd.begin(), helpCmd.end(), [](char & c) {c = ::tolower(c);});
                printHelp(helpCmd);
            }
            else 
                printHelp();
        }
        else if (cmd == "time")
            printCurrentTimeStep();
        else if (cmd == "prod")  
            printknownProducts();
        else if (cmd == "min")  
            processPrintMinOrderInput(params);
        else if (cmd == "max")  
            processPrintMaxOrderInput(params);
        else if (cmd == "avg")  
            processAvgOrdersInput(params);
        else if (cmd == "predict")  
            processPredictOrdersInput(params);
        else if (cmd == "wallet")
            printWallet();
        else if (cmd == "ask")
            enterAsk(params);
        else if (cmd == "bid")
            enterBid(params);
        else if (cmd == "step")  
            gotoNextTimeframe();
        else if (cmd == ".clear")
            system("clear");
        else 
            throw "Uncaught ReferenceError: " +  cmd +  " is not defined\n";
    }
}

void MerkelMain::printHelp(std::string cmd)
{
    if (cmd == "all") 
    {
        std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
        cout << "List of available commands: " << endl;
        cout << "help       Print this help message" << endl;
        cout << "help cmd   Output help for the specified command" << endl;
        cout << "time       returns the current time step" << endl;
        cout << "prod       returns the name of all available products found in the dataset" << endl;
        cout << "min        find minimum bid or ask for product in current time step" << endl;
        cout << "max        find maximum bid or ask for product in current time step" << endl;
        cout << "avg        compute average ask or bid of a product over the a number of time steps" << endl;
        cout << "predict    predict max or min of ask or bid for a sent product for the next time step" << endl;
        cout << "wallet     Prints the wallet with all your currencies " << endl;
        cout << "ask        make an offer to sell currency e.g \"ask ETH/BTC 200 0.5\"" << endl;
        cout << "bid        make an offer to buy currency e.g \"bid ETH/BTC 200 0.5\"" << endl;
        cout << "step       move to next time step" << endl;
        cout << ".clear     clears the console output to improve clarity" << endl;
    }
    else if (cmd == "time") 
    {
        cout << "time -> prints the current time step in YYYY/MM/DD HH:MM:SS format" << endl;
    }
    else if (cmd == "prod")
    {
        cout << "prod -> returns a list of all available products found in the dataset, e.g \"ETH/BTC,DOGE/BTC\"" << endl;
    }
    else if (cmd == "min")
    {
        cout << "min product bid/ask -> e.g: 'min ETH/BTC ask'  outputs \"The min ask for ETH/BTC is X\"" << endl;
    }
    else if (cmd == "max")
    {
        cout << "max product bid/ask -> e.g: 'max ETH/BTC ask'  outputs \"The max ask for ETH/BTC is X\"" << endl;
    }
    else if (cmd == "avg")
    {
        cout << "avg product ask/bid timesteps -> e.g: 'avg ETH/BTC ask 10' outputs \"The average ETH/BTC ask price over the last X timesteps was N\"" << endl;
    }
    else if (cmd == "predict")
    {
        cout << "predict max or min of ask or bid for a sent product for the next time step" << endl;
        cout << "predict max/min product ask/bid -> e.g: 'predict max ETH/BTC bid' outputs \"The average ETH/BTC ask price over the last 10 timesteps was 1.0\"" << endl;
    }
    else if (cmd == "wallet")
    {
        cout << "Prints the wallet with all your currencies. " << endl;
    }
    else if (cmd == "ask")
    {
        cout << "ask product price amount ->   e.g: 'ask ETH/BTC 200 0.5'  makes an offer to sell ETH for BTC, the price offered is 200, and the amount you're selling is 0.5 ETH." << endl;
    }
    else if (cmd == "bid")
    {
        cout << "bid product price amount ->   e.g: 'bid ETH/BTC 200 0.5'  makes an offer to buy ETH in exchange of BTC, price you're offering is 200, and the amount you're trying to purchase is 0.5 ETH." << endl;
    }
    else if (cmd == "step")
    {
        cout << "step -> \"now at 2020/03/17 17:01:30\"" << endl;
    }
    else if (cmd == ".clear")
    {
        cout << ".clear -> clears the console output to improve reading experience" << endl;
    }
    else
        printHelp();
}


void MerkelMain::printMinimum(std::string product, OrderBookType orderType)
{
    std::vector<OrderBookEntry> entries = orderBook.getOrders(orderType,
                                                            product,
                                                            currentTime);

    if (orderType == OrderBookType::ask || orderType == OrderBookType::asksale)
        std::cout << "The min ask for " << product << " is " << OrderBook::getLowPrice(entries) << std::endl;
    else 
        std::cout << "The min bid for " << product << " is " << OrderBook::getLowPrice(entries) << std::endl;
}

void MerkelMain::printMaximum(std::string product, OrderBookType orderType)
{
    std::vector<OrderBookEntry> entries = orderBook.getOrders(orderType,
                                                            product,
                                                            currentTime);

    if (orderType == OrderBookType::ask || orderType == OrderBookType::asksale)
        std::cout << "The min ask for " << product << " is " << OrderBook::getHighPrice(entries) << std::endl;
    else 
        std::cout << "The min bid for " << product << " is " << OrderBook::getHighPrice(entries) << std::endl;
}

void MerkelMain::enterAsk(std::vector<string> params)
{
    if (params.size() != 4)
    {
        std::ostringstream input;
        copy(params.begin(), params.end()-1, ostream_iterator<string>(input, ", "));
        std::cout << "MerkelMain::enterAsk Bad input! " << input.str() << std::endl;
    }
    else {
        try {//ask product price amount
            OrderBookEntry obe = CSVReader::stringsToOBE(
                params[2],
                params[3], 
                currentTime, 
                params[1], 
                OrderBookType::ask 
            );
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterAsk Bad input " << std::endl;
        }   
    }
}

void MerkelMain::enterBid(std::vector<string> params)
{
    if (params.size() != 4)
    {
        std::ostringstream input;
        copy(params.begin(), params.end()-1, ostream_iterator<string>(input, ", "));
        std::cout << "MerkelMain::enterBid Bad input! " << input.str() << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                params[2],
                params[3], 
                currentTime, 
                params[1], 
                OrderBookType::bid
            );
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds. " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }   
    }
}

void MerkelMain::printWallet()
{
    std::cout << wallet.toString() << std::endl;
}
        
void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl; 
            if (sale.username == "simuser")
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }
        
    }

    currentTime = orderBook.getNextTime(currentTime);
}

void MerkelMain::processPredictOrdersInput(vector<string> params) 
{
    int numberOfSteps = 0;
    double totalAverage = 0;

    vector<string> timesteps = orderBook.getKnownTimeSteps();
    vector<string> products = orderBook.getKnownProducts();

    if (params.size() < 4)
     {   
        printHelp("predict");
        return;
    }
    string function = params[1]; //Captures wether the user wants to predict min or max on the next time frame for a given product
    string product = params[2]; //captures the product the user wants to 
    OrderBookType orderType = OrderBookEntry::stringToOrderBookType(params[3]);
        
    // Confirm that the product and orderType strings typed by the user are valid
    if
        ((find(products.begin(), products.end(), product) != products.end()) &&
        (orderType == OrderBookType::ask || orderType == OrderBookType::bid) &&
        (function == "max" || function == "min"))
    {
        for (int i = 0; i < timesteps.size(); i++)
        {
            numberOfSteps++;

            vector<OrderBookEntry> orders = orderBook.getOrders(orderType, product, timesteps[i]);
            if (function == "min") 
                totalAverage += OrderBook::getLowPrice(orders);
            else 
                totalAverage += OrderBook::getHighPrice(orders);
            
            if (timesteps[i] == currentTime)
                break;

        }
        totalAverage = totalAverage / numberOfSteps;

        cout << "The predicted "<< function << " " << product << " "<< params[3] << " price for the next time step is ";
        cout << to_string(totalAverage) << endl;
        return;
    }
    else
    {
        printHelp("avg");
        return;
    }
}

void MerkelMain::processAvgOrdersInput(vector<string> params) 
{
    int numberOfSteps;
    vector<string> products = orderBook.getKnownProducts();
    if (params.size() < 3)
     {   
        printHelp("avg");
        return;
    }

    string product = params[1];
    OrderBookType orderType = OrderBookEntry::stringToOrderBookType(params[2]);
    if (params.size() >= 4 )
    {
        try
        {
            numberOfSteps = stoi(params[3]);
        }
        catch (const std::invalid_argument& ex)
        {
            cerr << "Invalid argument: " << ex.what() << '\n';
            printHelp("avg");
            return;
        }
    }
    else
        numberOfSteps = 0;
    
    // Confirm that the product and orderType strings typed by the user are valid
    if
        ((find(products.begin(), products.end(), product) != products.end()) &&
    (orderType == OrderBookType::ask || orderType == OrderBookType::bid))
    {
        vector<double> compoundAvg = computeAvgOrderOverNumberOfSteps(product, orderType, numberOfSteps);
        double totalAverage = compoundAvg[0];
        int lastSteps = (int) compoundAvg[1];
        cout << "The average " << product << " " << params[2] << " ";
        cout << "price over the last " << to_string(lastSteps) << " timesteps was " << to_string(totalAverage) << endl;
        return;
    }
    else
    {
        printHelp("avg");
        return;
    }
}

vector<double> MerkelMain::computeAvgOrderOverNumberOfSteps(std::string product, OrderBookType orderType, int numberOfSteps)
{
    vector<string> timesteps = orderBook.getKnownTimeSteps();
    auto itr = find(timesteps.begin(), timesteps.end(), currentTime);

    int currentTimeIndex = itr - timesteps.begin();
    int startIndex = 0;
    int numberOfConsideredSteps = 0;
    double sumOfAverages = 0;

    if ((currentTimeIndex - numberOfSteps) < 0 )
        startIndex = 0;
    else
        startIndex = currentTimeIndex - numberOfSteps; 

    for (int i = startIndex; i <= currentTimeIndex; i++)
    {  
        numberOfConsideredSteps++;
        vector<OrderBookEntry> orders = orderBook.getOrders(orderType, product, timesteps[i]);
        sumOfAverages += OrderBook::getAvgPrice(orders);
    }
    vector<double> result = {(sumOfAverages / numberOfConsideredSteps), (double) numberOfConsideredSteps};
    return result;
}


void MerkelMain::printCurrentTimeStep() 
{
    cout << currentTime << endl;
}

void MerkelMain::printknownProducts() 
{   
    vector<string> allProducts = orderBook.getKnownProducts();
    for_each(allProducts.begin(), allProducts.end()-1, [](string product) {cout << product << ",";});
    cout <<  allProducts[allProducts.size()-1] << endl;    
}

void MerkelMain::processPrintMinOrderInput(std::vector<std::string> params)
{
    vector<string> products = orderBook.getKnownProducts();
        // Checks if the tokenise function was able to capture three parameters, the command, product and orderType 
        // If not, prints help for the min command
        if (params.size() >= 3)
        {
            string product = params[1];
            OrderBookType orderType = OrderBookEntry::stringToOrderBookType(params[2]);
            // Confirm that the product and orderType strings typed by the user are valid
            if
                ((find(products.begin(), products.end(), product) != products.end()) &&
                (orderType == OrderBookType::ask || orderType == OrderBookType::bid))
            {
                printMinimum(product, orderType);
                return;
            }
        }
    printHelp("min");
}

void MerkelMain::processPrintMaxOrderInput(std::vector<std::string> params)
{
    vector<string> products = orderBook.getKnownProducts();
        // Checks if the tokenise function was able to capture three parameters, the command, product and orderType 
        // If not, prints help for the min command
        if (params.size() >= 3)
        {
            string product = params[1];
            OrderBookType orderType = OrderBookEntry::stringToOrderBookType(params[2]);
            // Confirm that the product and orderType strings typed by the user are valid
            if
                ((find(products.begin(), products.end(), product) != products.end()) &&
                (orderType == OrderBookType::ask || orderType == OrderBookType::bid))
            {
                printMaximum(product, orderType);
                return;
            }
        }
    printHelp("max");
}