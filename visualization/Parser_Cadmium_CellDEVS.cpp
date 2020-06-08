#include <iostream>
#include <fstream>
#include <string>
#include <regex>           //Read time separators 
#include <list>
#include <vector>
#include <bits/stdc++.h>   //Tokenize
#include <sys/stat.h>


using namespace std;

void print_list(list<string> list_in){
   for (auto const& i: list_in) {
        std::cout << i << "\n";
    }   
}

std::string conv_sec_to_time(int time_in){
   int hours =   (time_in / 3600 );
   int min = (time_in - hours*3600 ) / 60;
   int sec = (time_in - (hours*3600) - (min*60));
   std::string total;
   std::string hours_str;
   std::string min_str;
   std::string sec_str;
   if (hours<10){
      hours_str="0"+std::to_string(hours);
   }else{
      hours_str=std::to_string(hours);
   }
   if (min<10){
      min_str="0"+std::to_string(min);
   }else{
      min_str=std::to_string(min);
   }
   if (sec<10){
      sec_str="0"+std::to_string(sec);
   }else{
      sec_str=std::to_string(sec);
   }
   total = hours_str + ":" +min_str + ":" + sec_str + ":000";  
   //std::cout << "IN: " << time_in <<". out: " << total << std::endl;
   return total;
}

int palette_levels=20;
std::vector<std::string> colour_palete={
                                          "[255, 102, 102]",
                                          "[255, 140, 102]",
                                          "[255, 179, 102]",
                                          "[255, 217, 102]",
                                          "[255, 255, 102]",
                                          "[217, 255, 102]",
                                          "[179, 255, 102]",
                                          "[153, 255, 102]",
                                          "[140, 255, 102]",
                                          "[102, 255, 102]",
                                          "[102, 255, 140]",
                                          "[102, 255, 179]",
                                          "[102, 255, 217]",
                                          "[102, 255, 255]",
                                          "[102, 217, 255]",
                                          "[102, 179, 255]",
                                          "[102, 140, 255]",
                                          "[102, 102, 255]",
                                          "[140, 102, 255]",
                                          "[179, 102, 255]"
                                       };

int main(int argc, char *argv[]){
   fstream file_in;
   std::string filename=argv[1];
   file_in.open(filename,ios::in); 
   std::string model=argv[2];
   // std::string out_port=argv[3];
   std::vector<std::string> out_ports;
   int number_of_files=std::stoi(argv[3]);
   for (int i = 0; i < number_of_files; i++){
      out_ports.push_back("out_port"+std::to_string(i));
   }

   // char *folder=argv[4];
   // int mkdir (*folder);
   string folder = argv[4];
    int status = mkdir(folder.c_str(),0777);

   ofstream simulationFile;
   simulationFile.open (folder+"/simulation.json");
   ofstream transitionsFile;
   transitionsFile.open (folder+"/transitions.csv");
   ofstream optionsFile;
   optionsFile.open (folder+"/options.json");
   optionsFile <<   "{\"diagram\": {\"width\": 900,\"height\": 600,\"aspect\": true},   \"grid\": {\"columns\": " << number_of_files << ",\"width\": 400,\"height\": 400,\"spacing\": 10,\"showGrid\": false,\"aspect\": true},  \"playback\": {\"speed\": 8,\"loop\": false,\"cache\": 10}}" << std::endl;

   int max_cell_x;
   int max_cell_y;
   float max_value;
   float min_value;

   list <string> lines;
   vector <string> tokens; 
   vector <string> tokens2; 
   vector <string> tokens3; 
   vector <string> tokens4; 
   vector <string> tokens5; 
   string intermediate; 
   std::string time_var;
   if (file_in.is_open()){   //checking whether the file is open
      string tp;
      int n=0;
      while(getline(file_in, tp)){ //read data from file object and put it into string.
         //std::cout<<  tp << std::endl;
         regex c("\\[.*"); 
         if (!regex_search(tp, c) ) {
            int time_sec = stoi(tp);
            time_var=conv_sec_to_time(time_sec);
            std::cout<<  "time: " << time_var << std::endl;
         }
         else
         {
            stringstream check1(tp); 
            while(getline(check1, intermediate, '{')) { 
               tokens.push_back(intermediate); 
            }
            stringstream check2(tokens[(int)tokens.size()-1]); 
            while(getline(check2, intermediate, '}')) { 
               tokens2.push_back(intermediate); 
            }
            stringstream check3(tokens2[0]); 
            while(getline(check3, intermediate, ';')) { 
               tokens3.push_back(intermediate); 
            }
            std::string cell=tokens3[0];
            std::string message=tokens3[1];
            message.erase ((int)message.size()-1,1);
            message.erase (0,2);
            //cout << "CELL: " <<  cell << ". Message: " << message << '\n'; 
            stringstream check4(message); 
            while(getline(check4, intermediate, ',')) { 
               tokens4.push_back(intermediate); 
            }            
            cell.erase ((int)cell.size()-2,2);
            cell.erase (0,1);
            stringstream check5(cell); 
            while(getline(check5, intermediate, ',')) { 
               tokens5.push_back(intermediate); 
            }
            // for(int i = 0; i < tokens4.size(); i++) {
               //cout << "Y,"<< time_var << "," << model << "," << tokens5[0] << "-" << tokens5[1] << "-0" << "," << out_port << "," << model << "," <<  tokens4[i] << '\n'; 
            // }  
            for (int i=0; i<number_of_files;i++){
               transitionsFile << "Y,"<< time_var << "," << model << "," << tokens5[0] << "-" << tokens5[1] << "-0" << "," << out_ports[i] << "," << model << "," <<  tokens4[i] << '\n'; 
               if (max_value<std::stod(tokens4[i])){
                  max_value=std::stod(tokens4[i]);
               }
               if (min_value>std::stod(tokens4[i])){
                  min_value=std::stod(tokens4[i]);
               }
               if (max_cell_x<std::stoi(tokens5[0])){
                  max_cell_x=std::stoi(tokens5[0]);
               }
               if (max_cell_y<std::stoi(tokens5[1])){
                  max_cell_y=std::stoi(tokens5[1]);
               }
            }
         }
         
         tokens.clear();
         tokens2.clear();
         tokens3.clear();
         tokens4.clear();
         tokens5.clear();
      }
   }
   simulationFile << "{\"name\": \""<< model << "\",\"simulator\": \"Cadmium\",\"type\": \"Cell-DEVS\",\"models\": [{\"name\": \""<< model << "\",\"ports\": [";
   for (int i=0; i<number_of_files;i++){
      simulationFile << "{\"name\": \"" << out_ports[i] << "\",\"type\": \"output\"}";
      if (i<(number_of_files-1)){
          simulationFile << ",";
      }
   }
   simulationFile << "]}],\"size\": [ " << max_cell_x+1 << "," <<max_cell_y+1 << ", 1 ], \"palette\": [";
   float limit1;
   float limit2;
   for (int j=0; j<palette_levels;j++){
      limit1=(float)((j)*((max_value - min_value))/(float)palette_levels);
      limit2=(float)((j+1)*((max_value - min_value))/(float)palette_levels);
      simulationFile << "{ \"begin\": "<<  limit1 << ",\"end\": " << limit2 << ",\"color\": " <<  colour_palete[j] << "}";
      if (j<(palette_levels-1)){
         simulationFile << ",";
      }
   }
   simulationFile  << "] , \"template\": null}\n";

   file_in.close(); 
   std::cout << " --- Done --- "<< std::endl;
}