/*第2题	物品库存管理--源代码及关键源代码注解如下：*/
// file operations
#include<iostream>
#include<conio.h>
#include<stdio.h>       //for gets and puts
#include<process.h>     //exit(1)
#include<fstream>      //file operations
#include<string.h>


using namespace std;

class Brand {
private:
    char brnd_name[20];
    int Brand_code;
    int quantity;
    float brnd_price;

public:
    void getdata();

    void showdata();
};

class Item {
private:
    char Item_name[10];
    int Item_code;
    char colour[10];
    int quantity;
    Brand brnd[3];

    //Private Member functions used internally
    void getdata();

    void showdata();

public:
    void add();

    void remove();

    void search();

    void showab();
};

//                            GET Brand DATA
//	 gets data about the Brand
//	 it it temperary storage

void Brand::getdata() {
    cout << "\n\nEnter Brand Name ";
    gets(brnd_name);
    cout << "\nEnter Brand Code ";
    cin >> Brand_code;
    cout << "\nEnter Quantity ";
    cin >> quantity;
    cout << "\nEnter Price";
    cin >> brnd_price;
}

//	SHOW Brand DATA
// shows the data of the Brand
void Brand::showdata() {
    cout << "\n\nName          :  ";
    puts(brnd_name);
    cout << "\n\nCode          :  ";
    cout << Brand_code;
    cout << "\n\nQuantity      :  ";
    cout << quantity;
    cout << "\nPrice           :  ";
    cout << brnd_price;
}

//	GET Item DATA

// gets data about the Item	it is temporary storage

void Item::getdata() {
    cout << "\nEnter Item Name ";
    gets(Item_name);
    cout << "\nEnter Item Code  ";
    cin >> Item_code;
    cout << "\nEnter Colour ";
    gets(colour);
    cout << "\nEnter Quantity ";
    cin >> quantity;
    cout << "\n\nEnter Brand 1 ";
    brnd[0].getdata();
    cout << "\n\nEnter Brand 2 ";
    brnd[1].getdata();
    cout << "\n\nEnter Brand 3 ";
    brnd[2].getdata();
}

//	SHOW Item DATA
//shows data about the Item
void Item::showdata() {
    cout << "\n\n\t\tItem Data";
    cout << "\n\nItem Name    :  ";
    puts(Item_name);
    //写一个字符串到标准的输出流（或缓冲区）-Write a string to stdout
    cout << "\n\nItem Code    :  ";
    cout << Item_code;
    cout << "\n\nColour   :  ";
    puts(colour);
    cout << "\n\nQuantity     :  ";
    cout << quantity << endl;
    getch();
    // clrscr();
    cout << "\n\nBrand 1" << endl;
    brnd[0].showdata();
    getch();
    //clrscr();
    cout << "\n\nBrand 2" << endl;
    brnd[1].showdata();
    getch();
    //clrscr();
    cout << "\n\nBrand 3" << endl;
    brnd[2].showdata();
}

//	Add Item
/* this function first gets data into temp storage then
   saves it on the hard disk
   */
void Item::add() {
    //clrscr();
    ofstream file("TIS.txt", ios::out | ios::ate); //open file for input
    if (!file) //if file could not be opened
    {
        cout << "Error Could Not Open File";
        getch();
        exit(1);
    }
    Item p1;
    p1.getdata(); //Get Data From User For Temp. Storage
    //   write data to hard disk
    file.write(reinterpret_cast<const char *>(&p1), sizeof(Item));
}

//              TRAVERSE
/*  this function shows the list of all the Brands */
void Item::showab() {
    //clrscr();
    ifstream file("TIS.txt", ios::in); //open file for output
    if (!file) //if file could not be opened
    {
        cout << "Error Could Not Open File";
        getch();
        exit(1);
    }
    Item p1;
    file.read(reinterpret_cast<char *>(&p1), sizeof(Item));
    //Store Data In Object
    while (!file.eof()) //Untill Contacts Present
    {
        //clrscr();
        p1.showdata(); //Display on Screen
        getch(); //Wait For Keyboard Input
        file.read(reinterpret_cast<char *>(&p1),
                  sizeof(Item)); //Read Next Contact
    }
}

//   SEARCH
void Item::search() {
    //clrscr();
    char name1[10];
    int code;
    int option;
    cout << "\n(1)Search By Name\n(2)Search By Code";
    cin >> option;
    if (option == 1) {
        cout << "\nEnter Item name  ";
        gets(name1);
        code = 0;
    } else {
        cout << "\nEnter Code of Item To Search  ";
        cin >> code;
        strcpy(name1, "null");
    }
    fstream file("TIS.txt", ios::in); //Open File For Output/Read
    if (!file) //If File Could Not Be Opened
    {
        cout << "Error Could Not Open File";
        getch();
        exit(1);
    }
    char flag = 'a'; //Flag To Check If Found
    Item p1;;
    file.read(reinterpret_cast<char *>(&p1), sizeof(Item));
    //Read Data To Object
    while (!file.eof()) //Untill There Are Contacts
    {
        if ((strcmp(p1.Item_name, name1) == 0) || (p1.Item_code == code)) //Compare
        {
            //clrscr();
            p1.showdata();
            getch();
            flag = 'z'; //Set Flag
            break; //Break Loop
        }
        file.read(reinterpret_cast<char *>(&p1),
                  sizeof(Item));
        //Read Next Contact
    }
    if (flag != 'z') //If Not Found
    {
        cout << ("Item Not Found");
        getch();
    }
}

//	REMOVE Item
// remove is a bit different first all the data except the Item to be deleted is saved in an other file called delete...
// then the original file is washed and all the data is copied back again

void Item::remove() {
    //clrscr();
    char name1[10];
    int code;
    int option;
    cout << "\n(1)Delete By Name\n(2)Delete By Code";
    cin >> option;
    if (option == 1) {
        cout << "\nEnter Item name  ";
        gets(name1);
        code = 0;
    } else {
        cout << "\nEnter Code of Item To Search  ";
        cin >> code;
        strcpy(name1, "null");
    }
    ifstream file("TIS.txt", ios::in);
    ofstream temp("delete.txt", ios::trunc);
    if (!file || !temp) {
        cout << "Could Not Open File";
        getch();
        exit(1);
    }
    file.seekg(0);
    temp.seekp(0);
    Item p1;
    file.read(reinterpret_cast<char *>(&p1), sizeof(Item));
    while (!file.eof()) {
        if ((strcmp(name1, p1.Item_name) == 0) || (code == p1.Item_code)) {
            file.read(reinterpret_cast<char *>(&p1), sizeof(Item));
        } else {
            temp.write(reinterpret_cast<char *>(&p1), sizeof(Item));
            file.read(reinterpret_cast<char *>(&p1), sizeof(Item));
        }
    }
    file.close();
    temp.close();
    ifstream temp1("delete.txt", ios::in);
    ofstream file1("TIS.txt", ios::trunc);
    temp1.seekg(0);
    file1.seekp(0);
    temp1.read(reinterpret_cast<char *>(&p1), sizeof(Item));
    while (!temp1.eof()) {
        file1.write(reinterpret_cast<char *>(&p1), sizeof(Item));
        temp1.read(reinterpret_cast<char *>(&p1), sizeof(Item));
    }

    temp1.close();
    file1.close();
}

//	MAIN FUNCTION
int main() {
    Item m;
    int choice;
    while (1) {
        //clrscr();
        cout << "\n\tThe Inventory  System (TIS)";
        cout << "\n\n(1)Add Item\n\n(2)Show All Data"
                << "\n\n(3)Search Item\n\n(4)Remove Item\n\n(5)Exit TIS";
        cout << "\n\nEnter Choice ";
        cin >> choice;
        switch (choice) {
            case 1:
                m.add();
                break;
            case 2:
                m.showab();
                break;
            case 3:
                m.search();
                break;
            case 4:
                m.remove();
                break;
            case 5: {
                exit(1);
            }
        }
    }
}
