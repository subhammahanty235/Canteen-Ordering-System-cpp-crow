#include<iostream>
#include<crow.h>
#include<sqlite3.h>
#include<crow/query_string.h>
#include<nlohmann/json.hpp>
#include<crow/mustache.h>
#include <fstream>
#include<ctime>

using namespace std;


static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int main() {
    //for backend Routing and server
    crow::SimpleApp app;   
    sqlite3* db;
    char* zErrMsg = 0;
    int rc;

    rc = sqlite3_open("canteenproj.db", &db);
    cout<< rc << endl;

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    }
    else {
        fprintf(stderr, "Opened database successfully\n");
        
    }

    // single use statements --------->

    //statement to create a new table for users

    std::string usersql = "CREATE TABLE USER ("
        "userId INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT, "
        "enrollment TEXT, "
        "emailid TEXT, "
        "mobileNumber TEXT, "
        "password TEXT, "
        "totalSpend INTEGER DEFAULT 0"
        ");";

    //statement to create table for admin
    std::string adminsql = "CREATE TABLE ADMIN ("
        "adminId INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT, "
        "password TEXT, "
        "emailid TEXT, "
        "mobileNumber TEXT, "
        "totalRevenue INTEGER"
        ");";

    //statement to create table for fooditem
    std::string fooditemsql = "CREATE TABLE FOODITEM ("
        "itemId INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT, "
        "price INTEGER "

        ");";

    std::string ordersql = "CREATE TABLE food_order ("
        "orderId INTEGER PRIMARY KEY AUTOINCREMENT, "
        "customer_id INTEGER, "
        "item_id INTEGER, "
        "price INTEGER, "
        "prepared INTEGER DEFAULT 0,"
        "collected INTEGER DEFAULT 0,"
        "order_name TEXT,"
        "orderDate TEXT"
        ");";

    // <---------------


    // code to create tables ---> 
    /*
    rc = sqlite3_exec(db,ordersql.c_str(), callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        fprintf(stdout, "Table created successfully\n");
    }
    sqlite3_close(db);

   */

    //backend part
    
    // Homepage 
    CROW_ROUTE(app, "/")
        .methods("GET"_method)([](const crow::request& req, crow::response& res) {

        std::string filePath = "menupage-template.html";

        // Open the file
        std::ifstream file(filePath);

        // Check if the file is opened successfully
        if (file.is_open()) {
            // Read the file contents into a string
            std::string htmlContent((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());

            // Close the file
            file.close();

            res.add_header("Content-Type", "text/html");

            //res.write(htmlContent);
            res.body = htmlContent;
            res.end();


        }
        else {
            res.write("FIle not found");
            res.end();
        }


            });
    
    //login request route, 
    CROW_ROUTE(app, "/api/user/login")
        .methods("POST"_method)([](const crow::request& req, crow::response& res) {
        nlohmann::json jsonBody = nlohmann::json::parse(req.body);
        std::string emailid = jsonBody["email"];
        std::string upassword = jsonBody["password"];
       

        int rc;
        sqlite3* db;
        rc = sqlite3_open("canteenproj.db", &db);
        if (rc != SQLITE_OK) {
            std::cout << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
            res.write("Error happened");
            return;
        }

        string sqlstat = "SELECT * FROM USER WHERE emailid = ?;";
        sqlite3_stmt* stmt;
        rc = sqlite3_prepare_v2(db, sqlstat.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            res.write("Error happened");
            return;
        }

        sqlite3_bind_text(stmt, 1, emailid.c_str(), -1, SQLITE_STATIC);
        //sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        //cout << rc << endl;
        if (rc == SQLITE_ROW) {
            //cout << "Login Successful" << endl;
            std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt,1));
            std::string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::string enrollment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            std::string mobile = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            std::string password= reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            std::string totalspend = reinterpret_cast<const char*>(sqlite3_column_text(stmt,6));

            //cout <<id <<"  " << name << "  " << email << "  " << enrollment << "  " << password << "  " << totalspend << endl;
            //cout << password << endl;
            //cout << upassword << endl;
            if (password == upassword) {
                nlohmann::json jsonResponse;
                jsonResponse["success"] = true;
                jsonResponse["id"] = id;
                jsonResponse["name"] = name;
                jsonResponse["email"] = email;
                jsonResponse["enrollment"] = enrollment;
                jsonResponse["mobile"] = mobile;
                jsonResponse["password"] = password;
                jsonResponse["totalspend"] = totalspend;

                res.write(jsonResponse.dump());
                res.end();


            }
            else {
                res.write("Wrong credentials");
                res.end();

            }
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return;
        }
        else {
            //cout << "Error occured" << endl;
            res.write("Login failed");
            res.end();
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return;
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
            });

    //Route to redirect to login page
    CROW_ROUTE(app, "/api/user/login")
        .methods("GET"_method)([](const crow::request& req, crow::response& res) {

        std::string filePath = "login-template.html";

        // Open the file
        std::ifstream file(filePath);

        // Check if the file is opened successfully
        if (file.is_open()) {
            // Read the file contents into a string
            std::string htmlContent((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());

            // Close the file
            file.close();

            

            res.add_header("Content-Type","text/html");
            
            //res.write(htmlContent);
            res.body = htmlContent;
            res.end();


        }
        else {
            res.write("File Not found");
            res.end();
        }
      
  });

    CROW_ROUTE(app, "/api/getuserinfo/<string>").methods("GET"_method)([](const crow::request& req, crow::response& res, string userId) {
        int rc;
        sqlite3* db;
        rc = sqlite3_open("canteenproj.db", &db);
        if (rc != SQLITE_OK) {
            std::cout << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
            res.write("Error happened");
            res.end();
            return;
        }

        sqlite3_stmt* stmt;
        std::string sqlstat = "SELECT * from USER WHERE userId = ?;";
        rc = sqlite3_prepare_v2(db, sqlstat.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            res.write("Error happened");
            res.end();
            return;
        }
        sqlite3_bind_text(stmt, 1, userId.c_str(), -1, SQLITE_STATIC);


        rc = sqlite3_step(stmt);

        if (rc == SQLITE_ROW) {
            //cout << "Login Successful" << endl;
            std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            std::string enrollment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::string mobile = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            
            std::string totalspend = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));

            std::cout << id << "  " << name << "  " << email << "  " << enrollment << "  " <<  "  " << totalspend << endl;
            //cout << password << endl;
            //cout << upassword << endl;

            nlohmann::json jsonResponse;
            jsonResponse["success"] = true;
            jsonResponse["id"] = id;
            jsonResponse["name"] = name;
            jsonResponse["email"] = email;
            jsonResponse["enrollment"] = enrollment;
            jsonResponse["mobile"] = mobile;
            
            jsonResponse["totalspend"] = totalspend;

            res.write(jsonResponse.dump());
            res.end();
        }
        
        });

    CROW_ROUTE(app, "/myprofile")
        .methods("GET"_method)([](const crow::request& req, crow::response& res) {

        std::string filePath = "userprofilepage-template.html";

        // Open the file
        std::ifstream file(filePath);

        // Check if the file is opened successfully
        if (file.is_open()) {
            // Read the file contents into a string
            std::string htmlContent((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());

            // Close the file
            file.close();

          
            res.add_header("Content-Type", "text/html");

            //res.write(htmlContent);
            res.body = htmlContent;
            res.end();


        }
        else {
            res.write("File Not found");
            res.end();
        }

            });
    CROW_ROUTE(app, "/admin")
        .methods("GET"_method)([](const crow::request& req, crow::response& res) {

        std::string filePath = "adminpage-template.html";

        // Open the file
        std::ifstream file(filePath);

        // Check if the file is opened successfully
        if (file.is_open()) {
            // Read the file contents into a string
            std::string htmlContent((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());

            // Close the file
            file.close();


            res.add_header("Content-Type", "text/html");

            //res.write(htmlContent);
            res.body = htmlContent;
            res.end();


        }
        else {
            res.write("File Not found");
            res.end();
        }

            });


    CROW_ROUTE(app, "/admin").methods("POST"_method)([](const crow::request& req, crow::response& res) {
        auto tokenkey = req.get_header_value("admin-id-token-key");
        if (tokenkey == "") {
            nlohmann::json responseJson;
            cout << "ijtnjnjnjjkjbjhbkjjnjjhjjhjhjjnkb" << endl;
            responseJson["success"] = false;
            responseJson["msg"] = "You don't have access to adminPanel!";
            cout << responseJson << endl;
            res.write(nlohmann::json(responseJson).dump());
            res.end();
            return;

        }
        nlohmann::json combinedJson;
        int rc;
        sqlite3* db;
        rc = sqlite3_open("canteenproj.db", &db);
        if (rc != SQLITE_OK) {
            std::cout << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
            res.write("Error happened");
            return;
        }
        sqlite3_stmt* stmt;

        std::string admindataStatment = "select * FROM ADMIN WHERE adminId = 8284;";
        rc = sqlite3_prepare_v2(db, admindataStatment.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            res.write("Error happened");
            res.end();
            return;
        }
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            nlohmann::json adminData;
            
            
            adminData["adminId"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            adminData["name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            adminData["emailId"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            adminData["mobile"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            adminData["revenue"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            cout << adminData << endl;
            combinedJson["adminDetails"] = adminData;
        }

        //getting the order details for admin

        std::string orderdatastatement = "SELECT food_order.* , User.name , User.emailid FROM food_order JOIN USER ON food_order.customer_id = USER.userId ;";
        rc = sqlite3_prepare_v2(db, orderdatastatement.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cout << "Error preparing statement(Order Data): " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            res.write("Error happened");
            res.end();
            return;
        }
        rc = sqlite3_step(stmt);
        std::vector<nlohmann::json> orderArray;
        while (rc == SQLITE_ROW) {
            nlohmann::json jsonObj;
            jsonObj["orderId"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            jsonObj["orderName"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            jsonObj["orderDate"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
            jsonObj["price"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            jsonObj["prepared"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            jsonObj["collected"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            jsonObj["customerName"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
            jsonObj["customerEmail"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
            
            cout << jsonObj << endl;

            orderArray.push_back(jsonObj);
            rc = sqlite3_step(stmt);
        }

        combinedJson["orders"] = orderArray;

        // getting all the food items for admin 
        std::string sqlstat = "SELECT * FROM FOODITEM";
        rc = sqlite3_prepare_v2(db, sqlstat.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            res.write("Error happened");
            return;
        }
        rc = sqlite3_step(stmt);
        std::vector<nlohmann::json> itemArray;
        while (rc == SQLITE_ROW) {
            nlohmann::json jsonObject;
            jsonObject["id"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            jsonObject["name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            jsonObject["price"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            itemArray.push_back(jsonObject);
            rc = sqlite3_step(stmt);
        }

        combinedJson["items"] = itemArray;


        //flag for checking if everything worked perfectly or not
        combinedJson["success"] = true;
        //final response
        res.write(nlohmann::json(combinedJson).dump());
        res.end();
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;



        });

    CROW_ROUTE(app, "/api/user/signup").methods("POST"_method)([](const crow::request& req, crow::response& res) {
        
        nlohmann::json jsonBody = nlohmann::json::parse(req.body);
        
        std::string name = jsonBody["name"];
       
        std::string email = jsonBody["email"];
        
        std::string enrollment = jsonBody["enrollment"];
        
        std::string mobile = jsonBody["mobile"];;
       
        std::string password = jsonBody["password"];
        
        int rc;
        sqlite3* db;
        rc = sqlite3_open("canteenproj.db", &db);
        
        string sqlstat = "INSERT INTO USER (name, enrollment , emailid , mobileNumber , password) VALUES(?,?,?,?,?)";
        sqlite3_stmt* stmt;
       
        rc = sqlite3_prepare_v2(db, sqlstat.c_str(),-1, &stmt, nullptr);
        
        if (rc != SQLITE_OK) {
            std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            res.write("Error happened");
            return;

        }
       
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, enrollment.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, mobile.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, password.c_str(), -1, SQLITE_STATIC);
       
        rc = sqlite3_step(stmt);
        
        if (rc != SQLITE_DONE) {
            nlohmann::json jsonResponse;
            jsonResponse["success"] = false;
            res.write(jsonResponse.dump());
            res.end();
            
            std::cout << "Error inserting data: " << sqlite3_errmsg(db) << std::endl;
        }
        else {
            nlohmann::json jsonResponse;
            jsonResponse["success"] = true;
            res.write(jsonResponse.dump());
            res.end();
            std::cout << "Data inserted successfully!" << std::endl;

        }
        
        sqlite3_finalize(stmt);

        // Close the database
        sqlite3_close(db);


   


        });
    
    CROW_ROUTE(app, "/api/user/signup").methods("GET"_method)([](const crow::request& req, crow::response& res) {

        std::string filePath = "signup-template.html";

        // Open the file
        std::ifstream file(filePath);

        // Check if the file is opened successfully
        if (file.is_open()) {
            // Read the file contents into a string
            std::string htmlContent((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());

            // Close the file
            file.close();
            

            res.add_header("Content-Type", "text/html");

            //res.write(htmlContent);
            res.body = htmlContent;
            res.end();


        }
        else {
            res.write("File Not Found");
            res.end();
        }
        
        });

    CROW_ROUTE(app, "/api/admin/login").methods("GET"_method)([](const crow::request& req, crow::response& res) {
        std::string filePath = "admin-login-template.html";

        // Open the file
        std::ifstream file(filePath);

        // Check if the file is opened successfully
        if (file.is_open()) {
            // Read the file contents into a string
            std::string htmlContent((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());

            // Close the file
            file.close();


            res.add_header("Content-Type", "text/html");

            //res.write(htmlContent);
            res.body = htmlContent;
            res.end();


        }
        else {
            res.write("File Not Found");
            res.end();
        }

        });

    CROW_ROUTE(app, "/api/admin/login").methods("POST"_method)([](const crow::request& req, crow::response& res) {
        nlohmann::json jsonBody = nlohmann::json::parse(req.body);
        std::string adminid = jsonBody["adminid"];
        std::string upassword = jsonBody["password"];
       
        int rc;
        sqlite3* db;
        rc = sqlite3_open("canteenproj.db", &db);
        if (rc != SQLITE_OK) {
            std::cout << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
            res.write("Error happened");
            return;
        }

        string sqlstat = "SELECT * FROM ADMIN WHERE emailid = 'admin@collegecanteen.com'";
        sqlite3_stmt* stmt;
        rc = sqlite3_prepare_v2(db, sqlstat.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            res.write("Error happened");
            return;
        }

        //sqlite3_bind_text(stmt, 1, emailid.c_str(), -1, SQLITE_STATIC);
        //sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        
        if (rc == SQLITE_ROW) {
            
            std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::string emailid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            std::string mobileNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            std::string revenue= reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            

            //cout << id << "  " << name << "  " << email << "  " << enrollment << "  " << password << "  " << totalspend << endl;
            
            if (id == adminid &&  password == upassword) {
                nlohmann::json jsonResponse;
                jsonResponse["success"] = true;
                jsonResponse["id"] = id;
                jsonResponse["name"] = name;
                jsonResponse["email"] = emailid;
                jsonResponse["revenue"] = revenue;
                jsonResponse["mobileNumber"] = mobileNumber;
                jsonResponse["isAdmin"] = true;

                
                
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                res.write(jsonResponse.dump());
                res.end();


            }
            else {
                nlohmann::json jsonResponse;
                jsonResponse["success"] = false;
                jsonResponse["message"] = "Wrong credentials";
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                res.write(jsonResponse.dump());
                res.end();

            }
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return;
        }
        else {
            cout << "Error occured" << sqlite3_errmsg(db) << endl;
            res.write("Login failed");
            res.end();
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return;
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);

        });

    //for admin
    CROW_ROUTE(app, "/api/item/new").methods("POST"_method)([](const crow::request& req, crow::response& res) {
        nlohmann::json jsonBody = nlohmann::json::parse(req.body);
        std::string name = jsonBody["name"];
        std::string price = jsonBody["price"];
        int rc;
        sqlite3* db;
        rc = sqlite3_open("canteenproj.db", &db);
        if (rc != SQLITE_OK) {
            std::cout << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
            nlohmann::json jsonResponse;
            jsonResponse["success"] = false;
            res.write(jsonResponse.dump());
            res.end();
            return;
        }
        sqlite3_stmt* stmt;
        
        std::string sqlstat = "INSERT INTO FOODITEM (name, price) VALUES(?,?)";
        rc = sqlite3_prepare_v2(db, sqlstat.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            nlohmann::json jsonResponse;
            jsonResponse["success"] = false;
            res.write(jsonResponse.dump());
            res.end();
            return;
        }
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, price.c_str(), -1, SQLITE_STATIC);
        

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE) {
            std::cout << "Error inserting data: " << sqlite3_errmsg(db) << std::endl;
            nlohmann::json jsonResponse;
            jsonResponse["success"] = false;
            res.write(jsonResponse.dump());
            res.end();
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return;
        }
        else {
            cout << "data Inserted " << endl;
            nlohmann::json jsonResponse;
            jsonResponse["success"] = true;
            res.write(jsonResponse.dump());
            res.end();
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return;
        }
        
        sqlite3_finalize(stmt);
        sqlite3_close(db);

        
        });

    //for admin and user both
    CROW_ROUTE(app, "/api/item/all").methods("GET"_method)([](const crow::request& req, crow::response& res) {
        int rc;
        sqlite3* db;
        rc = sqlite3_open("canteenproj.db", &db);
        if (rc != SQLITE_OK) {
            std::cout << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
            res.write("Error happened");
            return;
        }
        sqlite3_stmt* stmt;

        std::string sqlstat = "SELECT * FROM FOODITEM";
        rc = sqlite3_prepare_v2(db, sqlstat.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            res.write("Error happened");
            return;
        }
        rc= sqlite3_step(stmt);
        std::vector<nlohmann::json> jsonArray;
        while (rc == SQLITE_ROW) {
            nlohmann::json jsonObject;
            jsonObject["id"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            jsonObject["name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            jsonObject["price"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            jsonArray.push_back(jsonObject);
            rc = sqlite3_step(stmt);

        }
        res.write(nlohmann::json(jsonArray).dump());
        res.end();
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;

        });

    //for user
    CROW_ROUTE(app, "/api/order/new/<string>").methods("POST"_method)([](const crow::request& req, crow::response& res,string id) {
        nlohmann::json jsonBody = nlohmann::json::parse(req.body);
        std::string userId = jsonBody["userid"];
        
        string name, price;
        int rc;
        sqlite3* db;
        rc = sqlite3_open("canteenproj.db", &db);
        if (rc != SQLITE_OK) {
            std::cout << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
            res.write("Error happened");
            return;
        }
        sqlite3_stmt* stmt;
        
        // -----------> get details about the food item
        std::string sqlstat = "SELECT * FROM FOODITEM where itemId = ?;";
        rc = sqlite3_prepare_v2(db, sqlstat.c_str(), -1, &stmt, nullptr);
        
        if (rc != SQLITE_OK) {
            std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            res.write("Error happened");
            res.end();
            return;
        }
        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);
       

        rc = sqlite3_step(stmt);
        
        if (rc == SQLITE_ROW) {
            nlohmann::json jsonObject;
            string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            price = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

            // getting the current date ---->
            std::time_t currentDate = std::time(nullptr);

            std::tm localDate;

            localtime_s(&localDate , &currentDate);
            char currentdate[15];
            std::strftime(currentdate, sizeof(currentdate), "%d-%m-%Y", &localDate);
            cout << "name - " <<name<<"price is --"<<price<< endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            sqlite3* db;
            rc = sqlite3_open("canteenproj.db", &db);



            if (rc != SQLITE_OK) {
                std::cout << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
                res.write("Error happened");
                return;
            }
            sqlite3_stmt* stmt;
            sqlstat = "INSERT INTO food_order (customer_id , item_id , price ,order_name ,orderDate ) values (?,?,?,?,?);";
            rc = sqlite3_prepare_v2(db, sqlstat.c_str(), -1, &stmt, nullptr);

            if (rc != SQLITE_OK) {
                std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_close(db);
                res.write("Error happened");
                res.end();
                return;
            }
            sqlite3_bind_text(stmt, 1, userId.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, price.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 4, name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 5, currentdate, -1, SQLITE_STATIC);


            cout << "----"<<endl;
            rc = sqlite3_step(stmt);
            if (rc == SQLITE_DONE) {
                cout << "Order Placed" << endl;
                nlohmann::json jsonResponse;
                jsonResponse["success"] = true;
                res.write(jsonResponse.dump());
                res.end();
                
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                return;

            }
            else {
                res.write("Can't place order");
                cout << sqlite3_errmsg(db) << endl;
                res.end();
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                return;
            }

        }
        });

    //for user
    CROW_ROUTE(app, "/api/order/all").methods("POST"_method)([](const crow::request& req, crow::response& res) {
        nlohmann::json jsonBody = nlohmann::json::parse(req.body);
        std::string userId = jsonBody["userid"];
        int rc;
        sqlite3* db;
        rc = sqlite3_open("canteenproj.db", &db);
        if (rc != SQLITE_OK) {
            std::cout << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
            res.write("Error happened");
            return;
        }
        sqlite3_stmt* stmt;

        std::string sqlstat = "SELECT orderId, order_name, price, orderDate, prepared , collected FROM food_order WHERE customer_id = ?;";
        rc = sqlite3_prepare_v2(db, sqlstat.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            res.write("Error happened");
            return;
        }
        sqlite3_bind_text(stmt, 1, userId.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        std::vector<nlohmann::json> jsonArray;
        while (rc == SQLITE_ROW) {
            nlohmann::json jsonObject;
            jsonObject["id"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            jsonObject["order_name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            jsonObject["price"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            jsonObject["orderDate"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            jsonObject["prepared"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            jsonObject["collected"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            jsonArray.push_back(jsonObject);
            rc = sqlite3_step(stmt);

        }
        res.write(nlohmann::json(jsonArray).dump());
        res.end();
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;

     });

    //only for admin
    CROW_ROUTE(app, "/api/order/changeflag/<string>").methods("POST"_method)([](const crow::request& req, crow::response& res , string orderid) {
        
        int rc;
        sqlite3* db;
        rc = sqlite3_open("canteenproj.db", &db);
        if (rc != SQLITE_OK) {
            std::cout << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
            nlohmann::json jsonResponse;
            jsonResponse["success"] = false;
            res.write(jsonResponse.dump());
            res.end();
            return;
        }
        sqlite3_stmt* stmt;

        std::string sqlstat = "SELECT prepared , collected , price , customer_id FROM food_order WHERE orderId = ?;";
        rc = sqlite3_prepare_v2(db, sqlstat.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            nlohmann::json jsonResponse;
            jsonResponse["success"] = false;
            res.write(jsonResponse.dump());
            res.end();
            return;
        }
        sqlite3_bind_text(stmt, 1, orderid.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        
        if(rc == SQLITE_ROW) {
            nlohmann::json jsonObject;
            string prepared = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            string collected = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            string price = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            string customer_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            cout << prepared << "---------" << collected <<price<<"--------------" << customer_id<< endl;
            
            std::string newsqlstat;
            if (prepared == "0" && collected == "0") {
                newsqlstat = "UPDATE food_order SET prepared = 1 WHERE orderId = ?;";
                   
            }
            else if (prepared == "1" && collected == "0") {
                cout << "Executing" << endl;
                newsqlstat = "UPDATE food_order SET collected = 1 WHERE orderId = ?;";
            }
            else if (prepared == "1" && collected == "1") {
                res.write("Order already Collected");
                res.end();
            }
            sqlite3_stmt* stmt;
            rc = sqlite3_prepare_v2(db, newsqlstat.c_str(), -1, &stmt, nullptr);

            if (rc != SQLITE_OK) {
                std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_close(db);
                nlohmann::json jsonResponse;
                jsonResponse["success"] = false;
                res.write(jsonResponse.dump());
                res.end();
                return;
            }

             sqlite3_bind_text(stmt, 1, orderid.c_str(), -1, SQLITE_STATIC);


            rc = sqlite3_step(stmt);
            cout << "rc -- > " << rc << endl;
            if (rc == SQLITE_DONE) {
                
                sqlite3_finalize(stmt);

                if (prepared == "1" && collected == "0") {
                    newsqlstat = "UPDATE USER SET totalSpend = totalSpend + ? WHERE userId = ?;";
                    rc = sqlite3_prepare_v2(db, newsqlstat.c_str(), -1, &stmt, nullptr);
                    if (rc != SQLITE_OK) {
                        std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
                        sqlite3_close(db);
                        nlohmann::json jsonResponse;
                        jsonResponse["success"] = false;
                        res.write(jsonResponse.dump());
                        res.end();
                        return;
                    }
                    sqlite3_bind_text(stmt, 1, price.c_str(), -1, SQLITE_STATIC);
                    sqlite3_bind_text(stmt, 2, customer_id.c_str(), -1, SQLITE_STATIC);
                    

                    rc = sqlite3_step(stmt);
                    if (rc == SQLITE_DONE) {
                        sqlite3_finalize(stmt);
                        cout << "Updated user " << endl;
                        newsqlstat = "UPDATE ADMIN SET totalRevenue = totalRevenue + ? WHERE adminId = 8284;";
                        rc = sqlite3_prepare_v2(db, newsqlstat.c_str(), -1, &stmt, nullptr);
                        if (rc != SQLITE_OK) {
                            std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
                            sqlite3_close(db);
                            nlohmann::json jsonResponse;
                            jsonResponse["success"] = false;
                            res.write(jsonResponse.dump());
                            res.end();
                            
                            return;
                        }
                        sqlite3_bind_text(stmt, 1, price.c_str(), -1, SQLITE_STATIC);
                        

                        rc = sqlite3_step(stmt);
                        if (rc == SQLITE_DONE) {
                            sqlite3_finalize(stmt);
                            cout << "Updated admin" << endl;
                            sqlite3_close(db);

                            nlohmann::json jsonResponse;
                            jsonResponse["success"] = true;
                            res.write(jsonResponse.dump());
                            res.end();
                            
                        }
                    }
                }
                else if (prepared == "0" && collected == "0") {
                    nlohmann::json jsonResponse;
                    jsonResponse["success"] = true;
                    res.write(jsonResponse.dump());
                    res.end();
                }
            }
            else {
                
                std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
                nlohmann::json jsonResponse;
                jsonResponse["success"] = false;
                res.write(jsonResponse.dump());
                res.end();
                return;
            }
            
            
            
        }
        
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;

        });

    app.port(18080).run();


    


        



}