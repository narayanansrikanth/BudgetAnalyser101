#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits> 
#include <sstream>

std::string formatAmount(double v) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << v;
    return oss.str();
}

struct Entry {
    std::string type; 
    std::string category;
    double amount;
    std::string date;
};

std::vector<Entry> entries;

// categories 
const std::vector<std::string> incomeCategories = {"Salary", "Bonus", "Gift", "Other"};
const std::vector<std::string> expenseCategories = {"Rent", "Food", "Entertainment", "Utilities", "Other"};

// Utility function to get a valid double
double getValidDouble(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if(std::cin.fail() || value < 0) {
            std::cin.clear(); 
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            std::cout << "Invalid input. Please enter a positive number.\n";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}

//function to get a valid choice from 1 to maxOption
int getValidChoice(int maxOption, const std::string& prompt) {
    int choice;
    while (true) {
        std::cout << prompt;
        std::cin >> choice;
        if(std::cin.fail() || choice < 1 || choice > maxOption) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid choice. Try again.\n";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return choice;
        }
    }
}

// Load from file
void loadData() {
    std::ifstream file("finance.txt");
    if (!file) return; // no file yet
    Entry e;
    while (file >> e.type >> e.category >> e.amount >> e.date) {
        entries.push_back(e);
    }
    file.close();
}

// Save to file
void saveData() {
    std::ofstream file("finance.txt");
    if(!file) {
        std::cout << "Error saving data!\n";
        return;
    }
    for (auto &e : entries) {
        file << e.type << " " << e.category << " " << e.amount << " " << e.date << "\n";
    }
    file.close();
}

// Display categories and get selection
std::string selectCategory(const std::vector<std::string>& categories) {
    std::cout << "Select a category:\n";
    for(size_t i = 0; i < categories.size(); ++i) {
        std::cout << i+1 << ". " << categories[i] << "\n";
    }
    int choice = getValidChoice(categories.size(), "Enter choice: ");
    return categories[choice-1];
}

// Add Income or Expense
void addEntry(const std::string& type) {
    Entry e;
    e.type = type;
    if(type == "Income")
        e.category = selectCategory(incomeCategories);
    else
        e.category = selectCategory(expenseCategories);
    e.amount = getValidDouble("Enter amount: ");
    std::cout << "Enter date (YYYY-MM-DD): ";
    std::getline(std::cin, e.date);
    entries.push_back(e);
    std::cout << type << " added successfully!\n\n";
}

// View summary
void viewSummary() {
    double totalIncome = 0, totalExpense = 0;
    std::cout << "\n=== Finance Summary ===\n";
    std::cout << std::left << std::setw(15) << "Type"
              << std::setw(15) << "Category"
              << std::setw(10) << "Amount"
              << std::setw(12) << "Date" << "\n";
    std::cout << "-----------------------------------------\n";
    for (auto &e : entries) {
        std::cout << std::left << std::setw(15) << e.type
                  << std::setw(15) << e.category
                  << std::setw(10) << e.amount
                  << std::setw(12) << e.date << "\n";
        if (e.type == "Income") totalIncome += e.amount;
        else totalExpense += e.amount;
    }
    std::cout << "-----------------------------------------\n";
    std::cout << "Total Income : " << totalIncome << "\n";
    std::cout << "Total Expense: " << totalExpense << "\n";
    std::cout << "Balance      : " << totalIncome - totalExpense << "\n\n";

    // Optional ASCII bars for fun
    int maxLen = 30;
    int incomeBar = static_cast<int>((totalIncome/(totalIncome+totalExpense))*maxLen);
    int expenseBar = maxLen - incomeBar;
    std::cout << "Income  : " << std::string(incomeBar, '|') << "\n";
    std::cout << "Expense : " << std::string(expenseBar, '|') << "\n\n";
}

void generateHTML() {
    double totalIncome = 0, totalExpense = 0;
    for (auto &e : entries) {
        if (e.type == "Income") totalIncome += e.amount;
        else totalExpense += e.amount;
    }
    double balance = totalIncome - totalExpense;

    std::ofstream file("report.html");
    if (!file) {
        std::cout << "Error creating HTML file.\n";
        return;
    }

    // HTML template with Google Charts pie chart
    file << R"(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8">
    <title>Monthly Finance Report</title>
    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript">
      google.charts.load('current', {'packages':['corechart']});
      google.charts.setOnLoadCallback(drawChart);
      function drawChart() {
        var data = google.visualization.arrayToDataTable([
          ['Type', 'Amount'],
          ['Income', )" << totalIncome << R"(],
          ['Expense', )" << totalExpense << R"(]
        ]);

        var options = {
          title: 'Income vs Expense',
          pieHole: 0.4
        };

        var chart = new google.visualization.PieChart(document.getElementById('piechart'));
        chart.draw(data, options);
      }
    </script>
    <style>
      body { font-family: Arial; margin: 30px; background: #f3f3f3; }
      .card { background: white; padding: 20px; border-radius: 12px; margin-bottom: 20px; box-shadow: 0 4px 8px rgba(0,0,0,0.08);}
      h1 { text-align: center; }
      table { width: 100%; border-collapse: collapse; margin-top: 10px; }
      th, td { padding: 10px; border-bottom: 1px solid #ddd; text-align: left; }
      th { background: #e8e8e8; }
      .summary { font-size: 20px; font-weight: bold; }
      .income { color: #2e8b57; }
      .expense { color: #b22222; }
      .balance { color: #1e90ff; }
    </style>
  </head>
  <body>
    <h1>Monthly Finance Report</h1>

    <div class="card">
      <p class="summary income">Total Income: $)" << formatAmount(totalIncome) << R"(</p>
      <p class="summary expense">Total Expense: $)" << formatAmount(totalExpense) << R"(</p>
      <p class="summary balance">Balance: $)" << formatAmount(balance) << R"(</p>
    </div>

    <div class="card" id="piechart" style="width: 100%; height: 400px;"></div>

    <div class="card">
      <h2>All Entries</h2>
      <table>
        <tr>
          <th>Type</th>
          <th>Category</th>
          <th>Amount</th>
          <th>Date</th>
        </tr>
)";

    for (auto &e : entries) {
        file << "<tr>"
             << "<td>" << e.type << "</td>"
             << "<td>" << e.category << "</td>"
             << "<td>$" << formatAmount(e.amount) << "</td>"
             << "<td>" << e.date << "</td>"
             << "</tr>\n";
    }

    file << R"(
      </table>
    </div>
  </body>
</html>
)";

    file.close();
    std::cout << "HTML report created: report.html\n\n";

    // Open the HTML in default browser (Windows)
    system("start report.html");

}


int main() {
    loadData();
    int choice;
    do {
        std::cout << "=== Personal Finance Manager ===\n";
        std::cout << "1. Add Income\n2. Add Expense\n3. View Summary\n4. Export HTML Report\n5. Exit\n";
        choice = getValidChoice(5, "Enter choice: ");
        switch(choice) {
            case 1: addEntry("Income"); break;
            case 2: addEntry("Expense"); break;
            case 3: viewSummary(); break;
            case 4: generateHTML(); break;  // new option
            case 5: saveData(); std::cout << "Exiting...\n"; break;
        }
    } while(choice != 4);
    return 0;
}
