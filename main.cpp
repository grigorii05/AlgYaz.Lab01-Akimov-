#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <any>
using namespace std;

class Json {
	any data; // unordered_map<string, any> ob; или vector<any> ob;
public:
	// Конструктор из строки, содержащей Json-данные.
	Json(const std::string& s)
	{
		bool flag, flag2;
		unordered_map<string, any> Ob1;
		vector<any> Ob2;
		if (s[0] == '{') {
			flag = true; // Объект не массив
		}
		if (s[0] == '[') {
			flag = false; // Массив
		}
		if (!(s[0] == '{' || s[0] == '[')) throw exception("Неверный формат строки");
		void* pointer = nullptr;
		string key = "";
		double x; int a; // Для чтения вещественных и целых значений
			// Ищем ключ - строку

		int k1 = 0, k2;
		while (1) // Цикл поиска ключа и значения
		{
			if (flag) // Чиатем ключ
			{
				k1 = (int)s.find("\"", k1); // начало строки "
				if (k1 < 0) throw exception("Неверный формат строки");
				k2 = (int)s.find("\"", k1 + 1); // конец строки "
				if (k2 < 0) throw exception("Неверный формат строки");
				key = s.substr(k1 + 1, k2 - k1 - 1); // Извлекаем ключ
				k1 = (int)s.find(":", k2 + 1); // Ищем : после ключа
				if (k1 < 0) throw exception("Неверный формат строки");

			}
			k1++; // Символ за : или за [
			string sOb;
			// Цикл чтения значения
			while (1) // Перебираем символы разделители пропускаем
			{
				if (k1 >= s.size()) throw exception("Неверный формат строки");
				if (s[k1] == ',') if (flag) break; else k1++;
				if (s[k1] == '}') if (flag) break; else throw exception("Неверный формат строки");
				if (s[k1] == ']') if (!flag) break; else throw exception("Неверный формат строки");
				if (s[k1] != ' ' && s[k1] != '\n' && s[k1] != '\r' && s[k1] != '\t') // Ищем не разделитель
				{
					switch (s[k1]) // Определяем тип значения
					{
					case '{': // Это объект
						k2 = (int)s.find("}", k1 + 1); // конец объекта
						if (k2 < 0) throw exception("Неверный формат строки");
						sOb = s.substr(k1, k2 - k1 + 1); // Строка задающая объект
					//	any_cast<unordered_map<string, any> *>(data)->insert({ key, any_cast<any>(Json(sOb)) });
						if (flag) Ob1.insert({ key, Json(sOb) });
						else Ob2.push_back(Json(sOb));
						k2++; // Переход к след символу за }
						break;
					case '[': // Значение массив
					{
						k2 = (int)s.find("]", k1 + 1); // конец массива
						if (k2 < 0) throw exception("Неверный формат строки");
						sOb = s.substr(k1, k2 - k1 + 1); // Строка задающая объект
						if (flag) Ob1.insert({ key, Json(sOb) });
						else Ob2.push_back(Json(sOb));
						k2++; // Переход к след символу за ]
						break;
					}
					case '\"': // Значение строка
						k2 = (int)s.find("\"", k1 + 1); // конец строки
						if (k2 < 0) throw exception("Неверный формат строки");
						sOb = s.substr(k1 + 1, k2 - k1 - 1); // Значение - простая строка
						if (flag) Ob1.insert({ key, sOb });
						else Ob2.push_back(sOb);
						k2++; // Переход к след символу за " (концом строки)
						break;
					case 'f':   case 't':  // значение true или false
						if (s[k1] == 'f') flag2 = false;
						else flag2 = true;
						k2 = k1 + 1;
						while (isalpha(s[k2])) k2++; // Пропускаем символы - буквы
						if (flag) Ob1.insert({ key, flag2 });
						else Ob2.push_back(flag2);
						break;
					case 'n':   // null
						k2 = k1 + 1;
						while (isalpha(s[k2])) k2++; // Пропускаем символы - буквы
						if (flag) Ob1.insert({ key, pointer });
						else Ob2.push_back(pointer);
						break;
					default:
						// Читаем значение как число
						stringstream ss(s.substr(k1));
						ss >> x;
						if (ss.fail()) // Ошибка чтения
							throw exception("Неверный формат строки");
						// Проверяем целое число или вещественное
						a = x;
						if (flag)
						{
							if (a == x) Ob1.insert({ key, a }); // Целое
							else Ob1.insert({ key, x }); // Вещественное
						}
						else
							if (a == x) Ob2.push_back(a); // Целое
							else Ob2.push_back(x); // Вещественное

						k2 = k1 + 1;
						while (s[k2] >= '0' && s[k2] <= '9' || s[k2] == '.') k2++; // Пропускаем части числа цифры и .

					} // switch (s[k])
					k1 = k2;

				} // if (s[k] != ' ' && s[k] != '\n' && s[k] != '\r'); // Ищем не разделитель
				else k1++; // Символы разделители пропускаем
				if (s[k1] == ']') break;
			} // while(1)

			if (s[k1] == ']' || s[k1] == '}') break;
		} // while(1)

		if (flag) data = Ob1;
		else data = Ob2;
	}

	void print()
	{
		if (is_object()) // Печать объекта
		{
			cout << endl << "{";
			int i = 0;
			for (auto pos : any_cast<unordered_map<string, any>>(data))
			{
				if (i > 0) cout << ", ";
				cout << endl << pos.first << ": ";
				if (pos.second.type() == typeid(string)) cout << any_cast<string>(pos.second);
				if (pos.second.type() == typeid(int)) cout << any_cast<int>(pos.second);
				if (pos.second.type() == typeid(double)) cout << any_cast<double>(pos.second);
				if (pos.second.type() == typeid(bool)) cout << any_cast<bool>(pos.second);
				if (pos.second.type() == typeid(void*)) cout << "null";
				if (pos.second.type() == typeid(Json)) any_cast<Json>(pos.second).print();
				i++;
			}
			cout << endl << "}";
		}
		else // Печать массива
		{
			cout << endl << "[";
			int i = 0;
			for (auto pos : any_cast<vector<any>>(data))
			{
				if (i > 0) cout << ", ";
				if (pos.type() == typeid(string)) cout << any_cast<string>(pos);
				if (pos.type() == typeid(int)) cout << any_cast<int>(pos);
				if (pos.type() == typeid(double)) cout << any_cast<double>(pos);
				if (pos.type() == typeid(bool)) cout << any_cast<bool>(pos);
				if (pos.type() == typeid(void*)) cout << "null";
				if (pos.type() == typeid(Json)) any_cast<Json>(pos).print();
				i++;
			}
			cout << "]";
		}
	}


	// Метод возвращает true, если данный экземпляр содержит в себе JSON-массив. Иначе false.
	bool is_array() const
	{
		if (data.type() == typeid(vector<any>)) return true;
		else return false;
	}
	// Метод возвращает true, если данный экземпляр содержит в себе JSON-объект. Иначе false.
	bool is_object() const
	{
		if (data.type() != typeid(vector<any>)) return true;
		else return false;
	}

	// Метод возвращает значение по ключу key, если экземпляр является JSON-объектом.
	// Значение может иметь один из следующих типов: Json, std::string, double, bool или быть пустым.
	// Если экземпляр является JSON-массивом, генерируется исключение.
	std::any& operator[](const std::string& key)
	{
		if (typeid(data) == typeid(vector<any>)) throw exception("Неверный тип");
		return any_cast<unordered_map<string, any>>(data)[key];
	}

	// Метод возвращает значение по индексу index, если экземпляр является JSON-массивом.
	// Значение может иметь один из следующих типов: Json, std::string, double, bool или быть пустым.
	// Если экземпляр является JSON-объектом, генерируется исключение.
	std::any& operator[](int index)
	{
		if (typeid(data) != typeid(vector<any>)) throw exception("Неверный тип");
		return any_cast<vector<any>>(data)[index];
	}


	// Метод возвращает объект класса Json из строки, содержащей Json-данные.
	static Json parse(const std::string& s)
	{

	}

	// Метод возвращает объекта класса Json из файла, содержащего Json-данные в текстовом формате.
	static Json parseFile(const std::string& path_to_file)
	{

	}
};

int main()
{
	// Строки для тестирования
	string teststr = "{\
		\"lastname\" : \"Ivanov\",\
			\"firstname\" : \"Ivan\",\
			\"age\" : 25,\
			\"islegal\" : false,\
			\"marks\" : [\
				4, 5, 5, 5, 2, 3\
			],\
			\"address\" : {\
					\"city\" : \"Moscow\",\
						\"street\" : \"Vozdvijenka\"\
				}\
	}";
	string str1 = "[\"Hello1\", 1.5, \"Hello3\", 2]";
	string str2 = "{\"Hello\": true, \"World\": false}";

	Json Js1(teststr);

	Js1.print();
	return 0;
}
