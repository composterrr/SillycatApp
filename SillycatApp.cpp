

#include <iostream>
#include <string.h>

#include <crow.h>
#include <boost/asio.hpp>
#include <json.hpp>

#include <libpq-fe.h>


using namespace std;

class DB
{
    string host = "localhost",
           dbname, user, password;
        /*dbname = "myb", user = "postgres", password = "111222";*/
    PGconn* conn = nullptr;

public:

    bool conn_status()
    {
        if (PQstatus(conn) == CONNECTION_BAD)
            return 0;
        else return 1;
    }

    //Подключение к БД
    void connect()
    {
        string conninfo = "host=" + host + " dbname=" + dbname + " user=" + user + " password=" + password;
        conn = PQconnectdb(conninfo.c_str());
        if (!conn_status())
        {
            cerr << "Connection failed: " << PQerrorMessage(conn) << endl;
            PQfinish(conn);
        }
        return;
    };

    //Проверка на корректность sql-запроса + содержит ли ответ строки
    bool exec_correct(PGresult* res)
    {
        ExecStatusType status = PQresultStatus(res);
        if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK)
        {
            cerr << "Query failed: " << PQerrorMessage(conn) << endl;
            PQfinish(conn);
            return 0;
        }
        if (status != PGRES_COMMAND_OK && !PQntuples(res))
        {
            cout << "Empty result!" << endl;
            PQclear(res);
            //Очищение памяти не требуется
            return 0;
        }
        return 1;
    }

    //Добавить нового кота
    void add(string name, string pers, string photo)
    {
        char* query = "INSERT INTO kitties (name, personality, photo) VALUES ($1, $2, $3)";
        const char* paramValues[3] = { name.c_str(), pers.c_str(), photo.c_str() };
        PGresult* res = PQexecParams(conn, query, 3, nullptr, paramValues, nullptr, nullptr, 0);
        cout << query << endl;

        if (exec_correct(res)) cout << name << " added" << endl;
        PQclear(res);
    }

    //Проверка на возможность голосования
    bool can_vote(const char* ip)
    {
        //Прошло ли 24 часа с голосования пользователем?
        char* query = "SELECT (voted < NOW() - INTERVAL '1 day' OR voted IS NULL) FROM users WHERE ip = $1;";
        const char* paramValues[1] = { ip };
        PGresult* res = PQexecParams(conn, query, 1, nullptr, paramValues, nullptr, nullptr, 0);

        if (!exec_correct(res)) return 0;

        //Если голос пользователя СТАРШЕ 24 часов, он может голосовать
        if (strcmp(PQgetvalue(res, 0, 0), "t") == 0)
        {
            cout << "User " << ip << " can vote" << endl;
            return 1;
        }
        cout << "User " << ip << " has already voted recently" << endl;

        PQclear(res);
        return 0;
    }

    //Проголосовать за кота
    void vote(string ip, string id)
    {
        if (!can_vote(ip.c_str())) return;

        //Прибавляем лайк коту
        char* query = "UPDATE kitties SET likes = likes + 1 WHERE id = $1;";
        const char* param[1] = { id.c_str() };
        PGresult* res1 = PQexecParams(conn, query, 1, nullptr, param, nullptr, nullptr, 0);

        //Проверяем кол-во лайков у кота
        query = "SELECT likes FROM kitties WHERE id = $1;";
        param[0] = { id.c_str() };
        PGresult* res2 = PQexecParams(conn, query, 1, nullptr, param, nullptr, nullptr, 0);

        if (exec_correct(res1) && exec_correct(res2))
            cout << "Kitten #" << id << " has " << PQgetvalue(res2, 0, 0) << " likes" << endl;

        //Обновляем дату голосования
        query = "UPDATE users SET voted = CURRENT_TIMESTAMP WHERE ip = $1;";
        param[0] = { ip.c_str() };
        PGresult* res3 = PQexecParams(conn, query, 1, nullptr, param, nullptr, nullptr, 0);
        if (exec_correct(res3)) cout << "Date of voting updated for user " << ip << endl;

        PQclear(res1);
        PQclear(res2);
        PQclear(res3);
    }

    //Отобразить котов (за 24 часа/за все время, с/без сортировки по лайкам)
    void show(bool flag24h, bool by_likes)
    {
        string query = "SELECT (name, personality, likes) FROM kitties ";

        if (flag24h)
            query += " WHERE date >= NOW() - INTERVAL '1 day'";
        if (by_likes)
            query += " ORDER BY likes DESC; ";
        else query += " ORDER BY id ASC;";
        cout << query << endl;

        PGresult* res = PQexec(conn, query.c_str());

        if (exec_correct(res))
        {
            int rowCount = PQntuples(res);
            for (int i = 0; i < rowCount; i++)
            {
                const char* kitten = PQgetvalue(res, i, 0);
                cout << kitten << endl;
            }
        }
        PQclear(res);
    }

    //Удаление кота (НЕдоступно для пользователей)
    void del(string id)
    {
        char* query = "DELETE FROM kitties WHERE id = $1";
        const char* param[1] = { id.c_str() };
        PGresult* res = PQexecParams(conn, query, 1, nullptr, param, nullptr, nullptr, 0);

        if (exec_correct(res))
            cout << "Cat #" << id << " deleted" << endl;

        PQclear(res);
    }

    DB()
    {
        connect();
        cout << "Connection to " << dbname << " is " << conn_status() << endl;
    }

    DB(string base, string name, string pass)
    {
        dbname = base; user = name; password = pass;
        connect();
        cout << "Connection to " << dbname << " is " << conn_status() << endl;
    }

    ~DB()
    {
        PQfinish(conn);
    }
};

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF-8");

    string base, name, pass;
    cout << "Hi! Please enter your db name: ";
    cin >> base;
    cout << endl << "Your username: ";
    cin >> name;
    cout << endl << "Db password: ";
    cin >> pass;

    DB* myb = new DB(base, name, pass);

    if (!(myb->conn_status()))
    {
        delete myb;
        return 1;
    }

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]() {
        return "Hello, hey, hello!!!!";  // Ответ на запрос
        });

    //Новый кот
    CROW_ROUTE(app, "/add_cat").methods("POST"_method)([myb](const crow::request& req) {
        auto body = nlohmann::json::parse(req.body);
        string name = body["name"];
        string pers = body["pers"];
        string photo = body["photo"];

        myb->add(name, pers, photo);

        return crow::response(200);
            });

    //Отправить голос
    CROW_ROUTE(app, "/vote").methods("POST"_method)([myb](const crow::request& req) {
        auto body = nlohmann::json::parse(req.body);

        string id = body["id"];
        string ip = req.remote_ip_address;
        myb->vote(id, ip);

        return crow::response(200); 
            });


    //Отобразить котов
    CROW_ROUTE(app, "/kitties").methods("GET"_method)([myb](const crow::request& req) {
        bool time_filter = req.url_params.get("time"); 
        bool likes_filter = req.url_params.get("likes");

        myb->show(time_filter, likes_filter);

        return crow::response(200);
            });


    //Удалить кота
    CROW_ROUTE(app, "/del_cat").methods("DELETE"_method)([myb](const crow::request& req) {
        auto body = nlohmann::json::parse(req.body);
        string id = body["id"];
        myb->del(id);
        return crow::response(200);
            });


    app.port(8080).multithreaded().run();

    delete myb;
    return 0;
}

