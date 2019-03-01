#include <iostream>
#include <string>
#include <fstream>
#include <stack>
#include <cctype>



using namespace std;


char homepage_indicator = 'N';
std::stack<std::string>	stk_subject, stk_predicate, stk_object;
std::ofstream ofs;

std::string get_subject(string& triple);
std::string get_predicate(string& triple);
std::string get_object(string& triple);
void add_json_doc(std::stack<std::string>& stk_subject, std::stack<std::string>& stk_predicate, std::stack<std::string>& stk_object);
int main()
{
	

	
	string subject, predicate, object;

	std::ifstream in("./dataset/KG.nt");
	ofs.open ("./output/SNV.json", std::ofstream::out | std::ofstream::app);
	//locale utf8_locale(locale(), new codecvt_utf8<wchar_t>);
	//ofs.imbue( std::locale() );


	ofs << "[\n";


	std::string  triple;

	while (std::getline(in, triple))
	{

		subject   = get_subject(triple);
		predicate = get_predicate(triple);
		if ( predicate == "homepage")
			homepage_indicator = 'Y';
		object    = get_object(triple);

		if (stk_subject.empty())
			stk_subject.push(subject);
		
		else if ( stk_subject.top() != subject)
		{
			add_json_doc(stk_subject,stk_predicate,stk_object);
			stk_subject.push(subject);
			
		}
		
		stk_predicate.push(predicate);
		stk_object.push(object);
	}
		if (!stk_subject.empty())				
			add_json_doc(stk_subject,stk_predicate,stk_object);

		ofs << "\n";
		ofs << "{}]";
		ofs.close();


return 0;
}
//******************************************************
//
//******************************************************
void add_json_doc(std::stack<std::string>& stk_subject, std::stack<std::string>& stk_predicate, std::stack<std::string>& stk_object){
	//std::ofstream ofs;
	//ofs.open ("./ISWC/iswc.json", std::ofstream::out | std::ofstream::app);
	std::string temp_top_predicate;
	ofs << "{\n";
	ofs << "\t\"_id\":" << stk_subject.top() << ",\n";
	stk_subject.pop();


	while (stk_object.size() > 1 )
	{
		if (stk_predicate.size() > 1)
		{				
			temp_top_predicate = stk_predicate.top();
			stk_predicate.pop();
		}

		if ( !(stk_predicate.top() == temp_top_predicate))
		{
			ofs << "\t" << temp_top_predicate << ":" << stk_object.top() << ",\n";
			stk_object.pop();
		}else
		{
			
			ofs << "\t" << temp_top_predicate << ":[";
			ofs <<  stk_object.top() << ",";
	 		stk_object.pop();
			temp_top_predicate = stk_predicate.top();
			stk_predicate.pop();
			while (stk_predicate.top() == temp_top_predicate)
			{
				ofs <<  stk_object.top() << ",";
	 			stk_object.pop();
				temp_top_predicate = stk_predicate.top();
				stk_predicate.pop();
			}
			//stk_predicate.push(temp_top_predicate);
			ofs <<  stk_object.top() << "],\n";
	 		stk_object.pop();


		}
	}
	
	
	ofs << "\t" << stk_predicate.top() << ":" << stk_object.top() << "\n";
	stk_object.pop();
	stk_predicate.pop();
	ofs << "},\n";


}//add_json_doc



//******************************************************
//
//******************************************************
std::string get_subject(string& triple ){



	unsigned first, last;
	string subject;

	
		first = 0;
		last = triple.find(">");
		subject = triple.substr (0,last);
		triple = triple.substr (last+1,triple.length());
		while (first < subject.length())
		{
			first = subject.find("/");
			subject = subject.substr (first+1, subject.length());
			//cout << subject << endl;
		}

		subject = "\"" + subject + "\"";
		//cout << subject << endl;
return subject;
}

//******************************************************
//
//******************************************************
std::string get_predicate(string& triple ){



	unsigned first, last;
	string predicate;

		first = 0;
		last = triple.find(">");
		predicate = triple.substr (0,last);
		triple = triple.substr (last+1,triple.length());
		while (first <= predicate.length())
		{
			first = predicate.find("/");
			predicate = predicate.substr (first+1, predicate.length());
			//cout << subject << endl;
		}


		if ( predicate.find("#") < predicate.length() )
			predicate = predicate.substr ((predicate.find("#"))+1, predicate.length());

		predicate = "\"" + predicate + "\"";
		//cout << predicate << endl;

	
return predicate;
}






//******************************************************
//
//******************************************************



bool is_number(string str){

	for(size_t i=0; i < str.length() ; i++)
	{
		if (!isdigit( str[ i ] ))
			return false;
	}

return true;
}//is_number










//******************************************************
//
//******************************************************
std::string get_object(string& triple){


	unsigned first, last;
	string object;


		first = 0;
		

		if (homepage_indicator == 'Y')
		{
			first = triple.find("<");
			last = triple.find(">");
			object = triple.substr (first+1, last-3);
			object = "\"" + object + "\"";
			homepage_indicator = 'N';

		}else if ( (triple.find(">") < triple.length())  && (triple.find("^") > triple.length()) )
		{
			object = triple.substr (0,triple.find(">"));
			if (object.substr(object.length()-2,object.length()).find("/") <= object.length())
				object = triple.substr (0,triple.find(">")-1);


			while (first < object.length())
			{
				first = object.find("/");
				object = object.substr (first+1, object.length());
				//cout << subject << endl;
			}
				if ( object.find("#") <= object.length())
				{
					first = object.find("#");
					object = object.substr (first+1, object.length());
				}

				object = "\"" + object + "\"";
		}
		else if (triple.find(">") < triple.length()  && (triple.find("^") < triple.length()) )
		{
			first = triple.find("\"");			
			last  = triple.substr (first+1,triple.length()).find("\"");



			if (is_number(triple.substr (first+1,last)) || triple.substr (first+1,last).find(".") <= triple.substr (first+1,last).length() ) 
			{
				object = triple.substr (first+1,last);
				//cout << "number-->" << object << "\n";

			}else if ( triple.substr (first+1,last).find("-") <= triple.substr (first+1,last).length() )  
			{
				object = triple.substr (first+1,last);

		

					if (object.find("T") <= object.length())
						object = object.substr (0,object.find("T"));// + "\"";

					object = "{$date:\"" + object + "T00:00:00.000Z\"}";  //ISODate("2013-10-01T00:00:00.000Z"),
			
				
			
				/*
				if ( object.find("-") <= object.length())  //commented for numeric dates
				{
			
				//object = "{$date:" + object + "}";  //ISODate("2013-10-01T00:00:00.000Z"),

				//object = "ISODate(\"" + object + "T00:00:00.000Z\")";  //ISODate("2013-10-01T00:00:00.000Z"),
					object = "{$date:" + object + "T00:00:00Z}";  //ISODate("2013-10-01T00:00:00.000Z"),
				//object = "ISODate(\"" + object + "\")";  //ISODate("2013-10-01T00:00:00.000Z"),
				
				}

				*/
			}else
			{
				object = triple.substr (first,last+2);			
			}

		
			

					//object = "\"" + object + "\"";  //"attDate":{$date:"2013-11-20T23:32:18Z"}
			


			

		}else
		{
			first = triple.find("\"");
			object = triple.substr (first,triple.length()-2);
			if ( object.find("@") <= object.length())
			{
				object = triple.substr (first,triple.find("@")-1);
				
				stk_predicate.push("\"language\"");
				string tmp = triple.substr(triple.find("@")+1,2);
				tmp = "\""+ tmp +"\"";		
				stk_object.push(tmp);

			
			}

		}

		//cout << object << endl;


return object;
}




