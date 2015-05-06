#include <iostream>
#include <string>
//#include <filesystem>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include"adler32.h"

using namespace std;
using namespace boost::filesystem;
using namespace boost::algorithm;

path file_path_get()
{
	cout << "Enter your Folder path" << endl;
	path p;
	string s;
	getline(cin, s);
	p = s;
	while (!is_directory(p))
	{
		cout << "ERROR " << p << " is not valid path " << endl
			<< "Enter valid path" << endl;
		getline(cin, s);
		p = s;
	}
	return p;
}

unsigned long hash_counter(string file_path) 
{

	ifstream c_file;// for openning file and hash it 
	c_file.open(file_path, ifstream::in);
	string file_text = {};
	while(!c_file.eof())
	{
	 file_text += c_file.get();
	}
	int length = (int)file_text.size();
	char* v = (char*)file_text.c_str();
	unsigned char* str = (unsigned char*)v;
	c_file.close();
	return adler32(str, length);
}

void dir_runner(path main_p, ofstream & file_output)
{

	for (directory_iterator dir_itr(main_p);
		dir_itr != directory_iterator();
		++dir_itr)
    {
        if (is_directory(*dir_itr)) dir_runner(*dir_itr, file_output);
        path p = *dir_itr;
        if(p.filename().string()!=".DS_Store")
        {
            if (is_regular_file(*dir_itr))
            {
                file_output << p.string() << "\t\t" << file_size(*dir_itr/*p*/) << " bt"
                << "\t\t" << p.filename().string() << "\t\t" << hash_counter(p.string()) << endl;
            }
        }

	}
	
}

void directory_scanner()
{
	path main_p = file_path_get();
	cout << "Enter your TSV File Path" << endl;
	path p_;
	string s_;
	getline(cin, s_);
	p_ = s_;
	while (!is_regular_file(p_)) 
	{
		cout << "ERROR  " << p_ << " is not valid Json File path" << endl
			<< "Enter valid Json File path" << endl;
		getline(cin, s_);
		p_ = s_;
	}

	ifstream TSV_file;
	TSV_file.open(s_, ifstream::in);
	string file_text = {};
	while (!TSV_file.eof()) 
	{
		file_text += TSV_file.get();
	}
	TSV_file.close();

	//
	vector<string> v;
	vector<vector<string>> v_;
	vector<string>::iterator pd;
	
	split(v, file_text, is_any_of("\n"));
	for (pd = v.begin()+1; pd != v.end(); pd++) 
	{
		vector<string> v_child;
		split(v_child, *pd, is_any_of("\t"), token_compress_on);
		v_.push_back(v_child);
	}

	cout << "\n---------------\nNEW OR CHANGED FILES:\n" << endl;
	for (recursive_directory_iterator dir_itr(main_p);
		dir_itr != recursive_directory_iterator();
		++dir_itr)
	{
		if (is_regular_file(*dir_itr))
		{
            path p0 = *dir_itr;
            if(p0.filename().string()!=".DS_Store")
            {
                string s0 = p0.string();
                //s0 = s0.erase(0, main_p.string().length());
                
                int k = 0; 									// for checking the changed_file
                for (unsigned int i = 0; i < v_.size()-1; i++)
                {
                    if ( s0 == v_[i][0] && p0.filename().string() == v_[i][2])
                    {
                        if (hash_counter(p0.string()) != boost::lexical_cast<unsigned long>(v_[i][3])) std::cout << "File : " << p0.filename().string() << " was changed" << endl;
                        k = 1;
                        v_.erase(v_.begin()+i,v_.begin()+(i+1));
                    }
                }
                //find changed_file
                if (k == 0) cout << "File : " << p0.filename().string() << " is a new file" << endl;
            }
		}
	}

	//find delered_file
	cout << "\n---------------\nDELETED FILES:\n" << endl;
	for (unsigned int i = 0; i < v_.size()-1; i++)
	{
		/*if (v_[i][2] != "still_there")*/ cout << "File : " << v_[i][2] << endl;
	}
}

void Start(path main_p) 
{
	ofstream file_output;
	file_output.open("Tsv_MyProject.tsv");
	file_output << "File Path: \t\t\t\t" << "SIZE: \t\t" << "FILE NAME: \t\t" << "FILE HASH: " << endl;
	dir_runner(main_p, file_output);
	file_output.close();
}


int main()
{
	cout << "0----For creating a TSV file." << endl;
	cout << "1----For looking for the changes of the olds." << endl;
	int c;
	cin >> c;
	cin.get();
	if (c == 0) { path file_path = file_path_get(); Start(file_path); }
	else directory_scanner();
	cin.get();
	return 0;
}
