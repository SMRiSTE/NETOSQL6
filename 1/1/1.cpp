#include <iostream>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>
#include <Windows.h>
#pragma execution_character_set( "utf-8" )

class book;

class publisher {
public:
    std::string name;
    Wt::Dbo::collection<Wt::Dbo::ptr<book>> books;

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::field(a, name, "name");
        Wt::Dbo::hasMany(a, books, Wt::Dbo::ManyToOne, "id_publisher");
    }

};

class stock;

class book {
public:
    std::string title;
    Wt::Dbo::ptr<publisher> id_publisher;
    Wt::Dbo::collection<Wt::Dbo::ptr<stock>> stocks;

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::field(a, title, "title");
        Wt::Dbo::belongsTo(a, id_publisher, "id_publisher");
        Wt::Dbo::hasMany(a, stocks, Wt::Dbo::ManyToOne, "id_book");
    }
};

class shop;
class sale;

class stock {
public:
    int count = 0;
    Wt::Dbo::ptr<book> id_book;
    Wt::Dbo::ptr<shop> id_shop;
    Wt::Dbo::collection<Wt::Dbo::ptr<sale>> sales;

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::field(a, count, "count");
        Wt::Dbo::belongsTo(a, id_book, "id_book");
        Wt::Dbo::belongsTo(a, id_shop, "id_shop");
        Wt::Dbo::hasMany(a, sales, Wt::Dbo::ManyToOne, "id_stock");
    }
};

class shop {
public:
    std::string name;
    Wt::Dbo::collection<Wt::Dbo::ptr<stock>> stocks;

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::field(a, name, "name");
        Wt::Dbo::hasMany(a, stocks, Wt::Dbo::ManyToOne, "id_shop");
    }
};

class sale {
public:
    double price;
    std::string date_sale;
    Wt::Dbo::ptr<stock> id_stock;
    int count;

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::field(a, price, "price");
        Wt::Dbo::field(a, date_sale, "date_sale");
        Wt::Dbo::belongsTo(a, id_stock, "id_stock");
        Wt::Dbo::field(a, count, "count");
    }
};


void findShop(Wt::Dbo::Session& session, std::string namepublisher) {
	Wt::Dbo::ptr<publisher> findPublisher = session.find<publisher>().where("name = ?").bind(namepublisher);
	Wt::Dbo::collection<Wt::Dbo::ptr<book>> books = session.find<book>().where("id_publisher_id = ?").bind(findPublisher);
	std::vector<Wt::Dbo::collection<Wt::Dbo::ptr<stock>>> stocks;
	for (const auto& id : books) {
		stocks.push_back(session.find<stock>().where("id_book_id = ?").bind(id.id()));
	}
	std::vector<Wt::Dbo::collection<Wt::Dbo::ptr<shop>>> shops;
	for (const auto& col : stocks) {
		for (const auto& id : col) {
			shops.push_back(session.find<shop>().where("id = ?").bind(id->id_shop));
		}
	}
	std::cout << "Shops with books of " << namepublisher << ": \n";
	std::set<std::string> names;
	for (const auto& col : shops) {
		for (const auto& id : col) {
			names.insert(id->name);
		}
	}
	for (const auto& name : names) {
		std::cout << name << std::endl;
	}
}


int main()
{

    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "Hello World!\n";

    try {
        std::string connectionString = "host=localhost "
            "port=5432 "
            "dbname=HW6 "
            "user=postgres "
            "password=HORPUYER";

        std::unique_ptr<Wt::Dbo::backend::Postgres> conn;
        conn = std::make_unique<Wt::Dbo::backend::Postgres>(connectionString);

        Wt::Dbo::Session s;
        s.setConnection(move(conn));
        s.mapClass<publisher>("publisher");
        s.mapClass<book>("book");
        s.mapClass<stock>("stock");
        s.mapClass<shop>("shop");
        s.mapClass<sale>("sale");

        s.dropTables();
        s.createTables();

        Wt::Dbo::Transaction tx{ s };

		std::unique_ptr<publisher> Litres{ new publisher() };
		Litres->name = "Litres";
		Wt::Dbo::ptr<publisher> Litres_ptr = s.add(std::move(Litres));
		std::unique_ptr<publisher> Labirint{ new publisher() };
		Labirint->name = "Labirint";
		Wt::Dbo::ptr<publisher> Labirint_ptr = s.add(std::move(Labirint));
		std::unique_ptr<publisher> Eksmo{ new publisher() };
		Eksmo->name = "Eksmo";
		Wt::Dbo::ptr<publisher> Eksmo_ptr = s.add(std::move(Eksmo));


		std::unique_ptr <book> warAndPeace_1{ new book() };
		warAndPeace_1->title = "War and Peace";
		warAndPeace_1->id_publisher = Litres_ptr;
		Wt::Dbo::ptr<book> warAndPeace_1_ptr = s.add(std::move(warAndPeace_1));
		std::unique_ptr <book> warAndPeace_2{ new book() };
		warAndPeace_2->title = "War and Peace";
		warAndPeace_2->id_publisher = Labirint_ptr;
		Wt::Dbo::ptr<book> warAndPeace_2_ptr = s.add(std::move(warAndPeace_2));
		std::unique_ptr <book> warAndPeace_3{ new book() };
		warAndPeace_3->title = "War and Peace";
		warAndPeace_3->id_publisher = Eksmo_ptr;
		Wt::Dbo::ptr<book> warAndPeace_3_ptr = s.add(std::move(warAndPeace_3));
		std::unique_ptr <book> masterAndMargarita_1{ new book() };
		masterAndMargarita_1->title = "Master and Margarita";
		masterAndMargarita_1->id_publisher = Litres_ptr;
		Wt::Dbo::ptr<book> masterAndMargarita_1_ptr = s.add(std::move(masterAndMargarita_1));
		std::unique_ptr <book> masterAndMargarita_2{ new book() };
		masterAndMargarita_2->title = "Master and Margarita";
		masterAndMargarita_2->id_publisher = Eksmo_ptr;
		Wt::Dbo::ptr<book> masterAndMargarita_2_ptr = s.add(std::move(masterAndMargarita_2));
		std::unique_ptr <book> mertvieDushi_1{ new book() };
		mertvieDushi_1->title = "Mertvie dushi";
		mertvieDushi_1->id_publisher = Litres_ptr;
		Wt::Dbo::ptr<book> mertvieDushi_1_ptr = s.add(std::move(mertvieDushi_1));
		std::unique_ptr <book> mertvieDushi_2{ new book() };
		mertvieDushi_2->title = "Mertvie dushi";
		mertvieDushi_2->id_publisher = Labirint_ptr;
		Wt::Dbo::ptr<book> mertvieDushi_2_ptr = s.add(std::move(mertvieDushi_2));


		std::unique_ptr<shop> chitayGorod{ new shop() };
		chitayGorod->name = "chitayGorod";
		Wt::Dbo::ptr<shop> chitayGorod_ptr = s.add(std::move(chitayGorod));
		std::unique_ptr<shop> domKnigi{ new shop() };
		domKnigi->name = "domKnigi";
		Wt::Dbo::ptr<shop> domKnigi_ptr = s.add(std::move(domKnigi));
		std::unique_ptr<shop> knigi{ new shop() };
		knigi->name = "knigi";
		Wt::Dbo::ptr<shop> knigi_ptr = s.add(std::move(knigi));

		std::unique_ptr<stock> stockMoscow_1{ new stock() };
		stockMoscow_1->id_book = warAndPeace_1_ptr;
		stockMoscow_1->id_shop = chitayGorod_ptr;
		stockMoscow_1->count = 100;
		Wt::Dbo::ptr<stock> stockMoscow_ptr_1 = s.add(std::move(stockMoscow_1));
		std::unique_ptr<stock> stockMoscow_2{ new stock() };
		stockMoscow_2->id_book = masterAndMargarita_2_ptr;
		stockMoscow_2->id_shop = chitayGorod_ptr;
		stockMoscow_2->count = 120;
		Wt::Dbo::ptr<stock> stockMoscow_ptr_2 = s.add(std::move(stockMoscow_2));
		std::unique_ptr<stock> stockMoscow_3{ new stock() };
		stockMoscow_3->id_book = mertvieDushi_1_ptr;
		stockMoscow_3->id_shop = domKnigi_ptr;
		stockMoscow_3->count = 130;
		Wt::Dbo::ptr<stock> stockMoscow_ptr_3 = s.add(std::move(stockMoscow_3));
		std::unique_ptr<stock> stockMoscow_4{ new stock() };
		stockMoscow_4->id_book = warAndPeace_3_ptr;
		stockMoscow_4->id_shop = knigi_ptr;
		stockMoscow_4->count = 100;
		Wt::Dbo::ptr<stock> stockMoscow_ptr_4 = s.add(std::move(stockMoscow_4));

		std::unique_ptr<sale> sale_1{ new sale() };
		sale_1->price = 100;
		sale_1->date_sale = "2023-07-02";
		sale_1->id_stock = stockMoscow_ptr_1;
		sale_1->count = 10;
		s.add(std::move(sale_1));
		std::unique_ptr<sale> sale_2{ new sale() };
		sale_2->price = 90;
		sale_2->date_sale = "2023-07-02";
		sale_2->id_stock = stockMoscow_ptr_2;
		sale_2->count = 20;
		s.add(std::move(sale_2));
		std::unique_ptr<sale> sale_3{ new sale() };
		sale_3->price = 110;
		sale_3->date_sale = "2023-07-02";
		sale_3->id_stock = stockMoscow_ptr_3;
		sale_3->count = 30;
		s.add(std::move(sale_3));
		std::unique_ptr<sale> sale_4{ new sale() };
		sale_4->price = 115;
		sale_4->date_sale = "2023-07-02";
		sale_4->id_stock = stockMoscow_ptr_4;
		sale_4->count = 60;
		s.add(std::move(sale_4));
		std::string namePublisher;
		std::cout << "Input name of publisher: ";
		std::cin >> namePublisher;
		findShop(s, namePublisher);
		tx.commit();
    }
    catch (std::exception& e) {
        std::cout << e.what() << "\n";
    }
}