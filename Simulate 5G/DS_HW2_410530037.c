#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
//---------------GOOGLE STYLE-------------
//2479 C:\DSA\data structure program\hw2\DS_HW2_410530037.c

int **data;  // row is user_id, column is less than 5 , elements of data is the choice shape of user 
int **border;  //represent every space in source shape whether be used  
int **result;  // row is user_id, column is less than 3, elements of result is frequency , y_position ,x_position
int *choice;  //id is user_id , elements of choice is each user has how many choice 
bool *visit;  //use to check user_id whether be used
int *pack_order;  //the elements of pack_oder is user_id , array id more large user_id has more choice 

void Packing(int users,int mode, int total_frq, int total_time);
int FindMinChoiceUserId(int users);
bool ExceedBorder(int y_position , int x_position, int current_choice_f, int current_choice_t, int user_id, int total_time,int total_frq);
int FindFrequency(int user_id, int mode);
void RemainderPacking(int users, int total_frq, int total_time, int mode);

//finds userid who has the least choice of shape
int FindMinChoiceUserId(int users) {
  int max = INT_MAX;
  int id = 0;
  for(int i = 0; i < users; i++) {
    if(!visit[i]) {
      if(choice[i] < max) {
        max = choice[i];
        id = i;
      }
    }
  }
  visit[id] = true;
  return id;
}

// finds the user selected shape which lower left point whether could begin with(y_position,x_position)
bool ExceedBorder(int y_position , int x_position, int current_choice_f, int current_choice_t, int user_id, int total_time,int total_frq) {
  for(int i = y_position; i < y_position + current_choice_f; i++) {  //from (y_position,x_position) to move up frequency units
    for(int j = x_position ; j < x_position + current_choice_t; j++) {  //move time units to the right
      if(i > total_frq-1) return false;  //exceed top border
      if(j > total_time-1) return false;  //exceed right border
      if(border[i][j] == 1) return false;  //(i,j)space is be used
    }
  }  //if nor return false represents user shape can begin with (y_position,x_position) 
  result[user_id][0] = current_choice_f; 
  result[user_id][1] = y_position;
  result[user_id][2] = x_position;
  for(int i = y_position; i < y_position + current_choice_f; i++) {
    for(int j = x_position ; j < x_position + current_choice_t; j++) {
      border[i][j] = 1;
    }
  }
  return true;
}
//find which shape of user choice can most suitable source
int FindFrequency(int user_id, int mode) {
  for(int i = 5; 0 < i; i--) {  //bubble sort make the choice of user frequency from small to large
    for(int j = 0; j < i - 1; j++) {
      if(data[user_id][j] > data[user_id][j+1] &&  data[user_id][j] != -1 && data[user_id][j+1] != -1) {
        int tmp = data[user_id][j];
        data[user_id][j] = data[user_id][j+1];
        data[user_id][j+1] = tmp;
      }
    }
  }
  if(mode == 1) {  //source frequency longer than time
    for(int i = 0; i < 5; i++) {  //each user has up to five choices , if has less 5 choice will show -1
      if(data[user_id][i] == -1) {
        return data[user_id][i-1];  //the largest frequency of choice 
      } else if(data[user_id][4] != -1) {  //because use -1 to find if has 5 choice will has np -1 
        return data[user_id][4];
      }
    }
  } else if(mode == 2) {  //source frequency equal to time
    for(int i = 0; i < 5; i++) {
      if(data[user_id][i] == -1) {
        return data[user_id][i/2];  //the median frequency
      } else if(data[user_id][4] != -1) {
        return data[user_id][2];
      }
    }
  } else if(mode == 3) {  //source frequency shorter than time
    return data[user_id][0];  //the smallest frequency
  }
}

//not return value 
//arranges each users' shape to fit the (Y x X )resource
//first makes bottom full , and then records border
//then according to border to put each user's shape into free space

void Packing(int users, int mode, int total_frq, int total_time) {
  int current_choice_f, current_choice_t;  // the frequency or time of the user's current selected shape 
  int x_position = 0, y_position = 0;  //coordinate , begins with(0,0)
  int layer = 0;  // bottom
  for(int q = 0; q < users; q++) {  // q represents number of times (this time is not about shape)
    int user_id = FindMinChoiceUserId(users);
    pack_order[q] = user_id; 
    current_choice_f = FindFrequency(user_id, mode);//Find Frequency which most suitable source shape 
    current_choice_t = 16 / current_choice_f;
    if(layer == 0) { 
      if((current_choice_t + x_position <= total_time) && (current_choice_f + y_position <= total_frq)) {  //add frequency and time first to see if exceeds 
        result[user_id][0] = current_choice_f;
        result[user_id][1] = y_position;  // records the lower left point of shape , y_position is row , x_position is column
        result[user_id][2] = x_position;
        for(int i = y_position; i < y_position + current_choice_f; i++) {
          for(int j = x_position ; j < x_position + current_choice_t; j++) {  
            border[i][j] = 1;  //makes border be 1 to represent coordinate space be used up
          }
        }
        x_position += current_choice_t;  //in the bottom, no need to add frequency
      } else {
        layer++;
      }
    }
    bool b =false;  // be used to break double loop
    if(layer > 0) {  //not bottom
      for(y_position = 0; y_position < total_frq; y_position++) {  //from bottom to top 
        x_position = 0;
        for(;x_position < total_time; x_position++) {  //from left to right
          if(border[y_position][x_position] == -1) {  // find first point it is not be used 
            b = true;  
            break;
          }
        }
        if(b) break;
      }
      b = false;
      //ExceedBorder() return false represents still not find the lower left point(start backward move and start up) which can accommodate user shape 
      while(!ExceedBorder(y_position , x_position, current_choice_f, current_choice_t, user_id, total_time, total_frq)) {
        if(x_position < total_time) x_position++;  // x_position : 0 ~ (total_time - 1)
        if(x_position == total_time) {  // exceed right border
          if(y_position <= (total_frq - 2)) {  // y_position : 0 ~ (total_frq - 1) but it later will ++ so -2
            y_position++;
          } else {  // exceed border
            b = true;
            break;
          }
          x_position = 0;  //afrash to left 0
        }
        if(b) break;
      }
    }
  }
}

// this function makes user whose most sutiable frequency shape could not put into source
// have a second chance to put their other not most suitable shape into the remainder space
void RemainderPacking(int users,int total_frq,int total_time,int mode) {
  for(int i = users-1; i >= 0; i--) {  //back to front , because back user have more chance
    int user_id = pack_order[i];  //pack_order represent the relative size of the user's choice
    if(result[user_id][0] == -1) {  //user's selected shape not be use 
      int choice_number = choice[user_id]; //choice[user_id] represent user_id has how many choice
      int current = 0;  //ith element
      for(int i = 0; i < choice_number; i++) {  //make user every choice shape can be try  
        if(mode == 1) {  //longer frequency          
          current = choice_number - i - 1;
          if(current == choice_number - 1) continue;  //in Packing() , this choice shape is be tested
        } else if(mode == 2) {  //equal
          if(i == choice_number/2) continue;
          current = i;
        } else if(mode == 3) {  //longer time
          if(i == 0) continue;
          current = i;
        }
        int second_pack_frq = data[user_id][current];  
        int second_pack_time = 16 / second_pack_frq;

        //----------------This part program is similar to Packing-----------
        int y_position = 0;
        int x_position = 0;
        bool b= false;  // be used to break double loop
        for(;y_position < total_frq; y_position++) {  
          x_position = 0;
          for(;x_position < total_time; x_position++) {
            if(border[y_position][x_position] == -1) {
              b = true; 
              break;
            }
          }
          if(b) break;
        }
        b = false;   
        bool end = false; //end represent whether find the point can begin with , if true then break no need to do other choice of this user
        while(!(end = ExceedBorder(y_position , x_position, second_pack_frq, second_pack_time, user_id, total_time, total_frq))) {
          if(x_position < total_time) x_position++;
          if(x_position == total_time) {
            if(y_position <= (total_frq - 2)) {
              y_position++;
            } else {
              b = true;
              break;
            }
            x_position = 0;
          }
          if(b) break;
        }
        if(end) break;
      }
    }
  }
}

//frees all malloc space
void FreeAllSpace(int users, int total_frq) {
  free(choice);
  free(visit);
  free(pack_order);
  for(int i = 0; i < users; i++) {
    free(data[i]);
    free(result[i]);
  }
  free(data);
  free(result);
  for(int i = 0; i < total_frq; i++) {
    free(border[i]);
  }
  free(border);
}

int main() {
  int total_frq, total_time, users, mode;
  scanf("%d%d%d", &total_frq, &total_time, &users);

  if(total_frq > total_time) {
    mode = 1;
  } else if(total_frq == total_time) {
    mode = 2;
  } else if(total_frq < total_time) {
    mode = 3;
  }
  //--------------initialize array which will be used-----------------------
  data = malloc(sizeof(int *) * users);
  border = malloc(sizeof(int *) * total_frq);
  result = malloc(sizeof(int *) * users);
  choice = malloc(sizeof(int) * users);
  visit = malloc(sizeof(bool) * users);
  pack_order = malloc(sizeof(int) * users);

  for(int i = 0; i < users; i++) {
    data[i] = malloc(sizeof(int) * 5);
    result[i] = malloc(sizeof(int) * 3);
  }
  for(int i = 0; i < total_frq; i++) {
    border[i] = malloc(sizeof(int) * total_time);
  }
  for(int i = 0; i < users; i++) {
    choice[i] = -1;  //no probability will be -1 , every user at least has one choice 
    pack_order[i] = -1;
    visit[i] = false;  
    for(int j = 0; j < 5; j++) {
      data[i][j] = -1;  //-1 represents only have j choice
    }
    for(int j = 0; j < 3; j++) {
      result[i][j] = -1;  //-1 represents userid i shape is not be put into source shape
    }
  }
  for(int i = 0; i < total_frq; i++) {
    for(int j = 0; j < total_time; j++) {
      border[i][j] = -1;  //no put userid shape will show -1
    }
  }
  //--------------------------------------------------
  int user_id;
  char ch;  //use to scanf blankspace
  for(int j = 0; j < users; j++) {
    scanf("%d",&user_id);
    for(int i = 0; i < 5; i++) {
      if((ch=getchar()) == '\n') {
        choice[user_id] = i;  //user_id has i choice
        break;
      }
      scanf("%d%*c%*d",&data[user_id][i]);  //scanf YxX
      if(i == 4) choice[user_id] = 5;
    }
  }
  //--------------------------------------------------
  Packing(users, mode, total_frq, total_time);
  RemainderPacking(users,total_frq,total_time,mode);
  //--------------------------------------------------
  int accept_users = 0 ;// total number of user whose shape is put into source shape
  for(int i = 0; i < users; i++) {
    if(result[i][0] == -1) {
      continue;
    }
    accept_users++;
  }
  printf("%d\n",accept_users);
  for(int i = 0; i < users; i++) {
    if(result[i][0] == -1) {
      continue;
    }
    printf("%d %dx%d %d %d\n", i, result[i][0], 16 / (result[i][0]), result[i][1], result[i][2]);
  }
  FreeAllSpace(users, total_frq);
  return 0;
}