#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>

using namespace std;

const int MAXIMUM_ITEMS_PER_BILL = 50,
			MAXIMUM_NAME_LENGTH = 100,
			WARNING_LEVEL = 5,
			MAXIMUM_PRODUCTS_IN_STOCK = 1000;

//Only 50 items can be bought.
//Maximum length of name of a product is 100
//Will send a message if the stock of a product is less than 5
//THe godown can hold only 1000 items of a particular product

int Cmpstr(string a, string b){        //To compare 2 strings
    unsigned int sz = a.size();
    if (b.size() != sz)
        return false;
    for (unsigned int i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}

struct ProductInStock{                  //The object that defines the product details
	int pid;
	char name[MAXIMUM_NAME_LENGTH];
	float buyingPrice, sellingPrice;
	int quantity;
	char type;

	void print(){
		cout<<"Product ID "<<pid<<endl;
		cout<<"Name : "<<string(name)<<endl;
		cout<<"Buying price : "<<buyingPrice<<"\tSelling Price : "<<sellingPrice<<endl;
		cout<<"Quantity : "<<quantity<<"\nType : "<<type<<endl;
	}
};

struct Bill{                 //The object with which a customer can view the bill
	int bId, numberOfItems;
	char phoneNumber[12];
	int items[MAXIMUM_ITEMS_PER_BILL], 
		quantity[MAXIMUM_ITEMS_PER_BILL],
		cost[MAXIMUM_ITEMS_PER_BILL];
	float total;

	void print(){
		cout<<"Bill ID "<<bId<<endl;
		cout<<"Phone : "<<string(phoneNumber)<<endl;
		for(int i = 0; i < numberOfItems; i++){
			cout<<"Product ID : "<<items[i]<<"\tQuantity : "<<quantity[i]<<"\tCost : "<<cost[i]<<endl;
		}	
	}
};


class Market{             //The market
	private:
		ProductInStock products[MAXIMUM_PRODUCTS_IN_STOCK];
		int numberOfProducts;
		int numberOfBillsIssued;

		float Expenditure, Income;
	public:
		Market(){
			numberOfProducts = 0;
			ProductInStock s;
			fstream f("Product.DAT", ios::binary|ios::in);
			f.read((char*)&s,sizeof(s));            
			while(!f.eof()){			//reads all the product in stock from a file into local
				products[numberOfProducts++] = s;
				f.read((char*)&s,sizeof(s));
			}

			numberOfBillsIssued = 0;
			Bill r;
			fstream h("Billcopy.DAT", ios::binary|ios::in);    //Used to view an already existing bill
			h.read((char*)&r,sizeof(r));
			while(!h.eof()){		//reads all the bills from the file into local
				numberOfBillsIssued++;
				h.read((char*)&r,sizeof(r));
			}

			Income = 0.0;
			Expenditure = 0.0;
		}

		void addProducts(){				//Supplying products - new or old
			char repeat = 'Y';
			do{
				int pid, quantity;
				char name[MAXIMUM_NAME_LENGTH];
				float buyingPrice, sellingPrice;
				char type;

				bool newItem = true;

				cout<<"Enter the name of the product"<<setw(21)<<": ";
				scanf("%s",name);
				int i;
				for(i = 0; i < numberOfProducts; i++)        //If the entered item already exists
				{
					if(Cmpstr(products[i].name, name)){
						newItem = false;
						break;
                    }
				}
				cout<<"h = household \ng = grocery \nc = cosmetics"<<endl;
				cout<<"Enter the starting letter of the type"<<setw(13)<<": ";
				cin>>type;
				cout<<"Enter the buying price of the product"<<setw(13)<<": ";
				cin>>buyingPrice;
				cout<<"Enter the selling price of the product"<<setw(12)<<": ";
				cin>>sellingPrice;
				cout<<"Enter the quantity you want to supply"<<setw(13)<<": ";
				cin>>quantity;

				if(!newItem){                   //old
					char todo = 'Y';
					cout<<"Quantity will be added and details will be overwritten as product already exist.";
					cout<<"Do you want to overwrite? [Y/N]"<<setw(21)<<": ";
					cin>>todo;
					if(todo == 'N')
						continue;
					products[i].quantity += quantity;
				    strncpy(products[i].name, name, MAXIMUM_NAME_LENGTH);   //modifies the details of the product
				    products[i].buyingPrice = buyingPrice;
				    products[i].sellingPrice = sellingPrice;
				    products[i].type = type;
				}	
				else{             //new
					products[numberOfProducts].quantity = quantity;
				    strncpy(products[numberOfProducts].name, name, MAXIMUM_NAME_LENGTH);   //modifies the details of the product
				    products[numberOfProducts].pid = numberOfProducts;
				    products[numberOfProducts].buyingPrice = buyingPrice;
				    products[numberOfProducts].sellingPrice = sellingPrice;
				    products[numberOfProducts].type = type;
				    numberOfProducts++;
                }

				Expenditure += (buyingPrice*quantity);

				cout<<"Do you want supply another product [Y/N]?"<<setw(9)<<": ";
				cin>>repeat;
			}while(repeat == 'Y');
		}

		void Purchase(){      //If it is a consumer
			Bill b;

			numberOfBillsIssued++;

			int numberOfItemsBought = 0, quantity = 0, itemsAdded = 0;
			float total = 0.0, discount = 0.0;
			bool enterNumber;
			char pn[12] = "0000000000";

			char continueEnterItems = 'N';

			ofstream fout;
			fout.open("Bill.txt");
			fstream bc("Billcopy.DAT", ios::binary|ios::in|ios::out);
			fout<<"WELCOME TO HERITAGE SUPERMARKET"<<endl<<".............................................................................."<<endl;
			fout<<"Your bill no. is "<<numberOfBillsIssued<<endl;

			b.bId = numberOfBillsIssued;

			char input;
   			cout<<"Would you like to enter your mobile number [Y/N]"<<setw(3)<<": ";
			cin>>input;
			enterNumber = input == 'Y' ? true : false;
			if(enterNumber){
   				cout<<"Please enter your 10 digit mobile number"<<setw(11)<<": ";
				scanf("%s",b.phoneNumber);
				fstream con("Customers.DAT", ios::binary|ios::in);
		    	while(!con.eof()){
					con.read(reinterpret_cast<char*>(&pn), sizeof(pn));
					if(Cmpstr(string(pn), string(b.phoneNumber))){         //Existing customer means 10% discount
						cout<<"Welcome back! You avail 10% discount\n";
						fout<<"Welcome! You have been availed a discount of 10%"<<endl;
						discount = 0.1;
						break;
					}
				}
				con.close();
				if(discount == 0.0){
					fstream con("Customers.DAT", ios::binary|ios::out);
					cout<<"Written"<<endl;
					con.seekp(0, ios::end);
		    		con.write(reinterpret_cast<char*>(&b.phoneNumber), sizeof(b.phoneNumber));
		    		con.close();
		    	}
		    }

			cout<<"Enter the no. of products you want to buy"<<setw(10)<<": ";
			cin>>numberOfItemsBought;
			do{
				for(int i = 0; i < numberOfItemsBought; i++){
					string name;
					int quantityAvailable = -1, index;
					float costOfProduct;

					cout<<"Enter the name of the product"<<setw(21)<<": ";
					cin>>name;

					for(int j = 0; j < numberOfProducts; j++){
						if(Cmpstr(products[j].name, name)){
							quantityAvailable = products[j].quantity;
							index = j;
							break;
						}					
					}
					if(quantityAvailable == -1){
						cout<<"Sorry. We do not have the required product"<<endl;
						continue;
					}
					cout<<"Cost of the item is"<<setw(5)<<": "<<products[index].sellingPrice<<endl;
					cout<<"Enter the amount required"<<setw(25)<<": ";
					cin>>quantity;
					if(quantity > quantityAvailable){
						char option = 'Y';
						cout<<"Sorry. We have only "<<quantityAvailable<<" in stock. Do you want it to be billed [Y/N]"<<endl;
						cin>>option;
						if(option == 'Y')
							quantity = quantityAvailable;
						else
							continue;
					}
					fout<<i+1<<". Item = "<<name<<"\tQuantity = "<<quantity<<"\tCost = "<<products[index].sellingPrice<<endl;

					total = total + products[index].sellingPrice*quantity;
					b.items[itemsAdded] = products[index].pid;
					b.quantity[itemsAdded] = quantity;
					b.cost[itemsAdded] = products[index].sellingPrice;

					products[index].quantity -= quantity;
					itemsAdded++;
					if(products[index].quantity <= WARNING_LEVEL){
						ofstream f;
						f.open("Warning.txt", ios::app);
						if(products[index].type == 'g')
							fout<<"Buy "<<string(products[index].name)<<" from varada!";
						else if(products[index].type == 'c')
							fout<<"Buy "<<string(products[index].name)<<" from shiv!";
						else
							fout<<"Buy "<<string(products[index].name)<<" from desikan!";
					}
					cout<<"Item added"<<endl;
				}
				cout<<itemsAdded<<" items added. Do you still want to continue [Y/N]"<<endl;
				cin>>continueEnterItems;
				numberOfItemsBought=1; //Everytime it re-runs, it should occur only once
			}while(continueEnterItems == 'Y');
			total = total - (total*discount);
			Income += total;
			b.numberOfItems = itemsAdded;
			b.total=total;
			
			b.print();

			cout<<"The total is : "<<total<<endl;
			int amt = -1;
			while(total > 0 and amt < total){
				cout<<"Enter the amount paid (greater than or equal to total) : ";
				cin>>amt;
			}
			fout<<"The total is : "<<total<<endl;
			fout<<"The amount paid is : "<<amt<<endl;
			fout<<"The balance is : "<<amt-total<<endl;

			fout<<endl<<".............................................................................."<<endl;
			fout<<"THANK YOU. PLEASE VISIT AGAIN"<<endl;
			
			bc.seekp(0,ios::end);
			bc.write((char*)&b, sizeof(b));
			bc.close();
			fout.close();
			return;
		}

		void viewSavedBills(){
			int billNumber;
			cout<<"Enter the bill number. Total bills is "<<numberOfBillsIssued<<endl;
			cin>>billNumber;

			Bill s;
			fstream f("Billcopy.DAT", ios::binary|ios::in|ios::out);
			f.read((char*)&s,sizeof(s));
			while(!f.eof()){
				if(s.bId == billNumber){
					s.print();
					return;
				}
				f.read((char*)&s,sizeof(s));
			}
			f.close();
		}

		void run(){
			int option = 0;
			do{
				cout<<"Supplier [1]\nPurchase [2]\nViewProfit [3]\nView All Products [4]\nView Particular bill [5]\nExit [6]"<<endl;
				cin>>option;
				if(option == 1){
					addProducts();
				}
				else if(option == 2){
					Purchase();
				}
				else if(option == 3)
					viewProfit();
				else if(option == 4)
					viewAllPurchasedProducts();
				else if(option == 5)
					viewSavedBills();
				else 
					return;
			}while(option != 6);
			return;
		}

		void viewProfit(){
			cout<<"Total Income : "<<Income<<"\nTotal Expenditure : "<<Expenditure<<"\nTurnover : "<<Income - Expenditure<<endl;
		}

		void viewAllPurchasedProducts(){
			cout<<"TOTAL NUMBER OF PRODUCTS : "<<numberOfProducts<<endl<<endl;
			for(int i = 0; i < numberOfProducts; i++){
				products[i].print();
				cout<<"-------------------------------------------------------------------"<<endl;
			}
		}

		~Market(){
			fstream t;
			t.open("Product.DAT",ios::binary|ios::out);
			for(int i = 0; i < numberOfProducts; i++){
				t.write((char*)&products[i],sizeof(products[i]));	 
			}
			t.close();
		}
};

int main(){
	Market m;
	m.run();
	return 0;
}
