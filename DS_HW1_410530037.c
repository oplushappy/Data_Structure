#include <stdio.h>
#include <stdbool.h>
#include<stdlib.h>
#include<string.h>
#include <limits.h>
//----------GOOGLE STYLE------------------

struct Node {  //Node 有data 和 指向下一點的指標next
  int data;
  struct Node *next;
} ;

struct QNode {  //QNode 有分別指向第一個和最後一個的指標
  struct Node *front;  //front is a pointer
  struct Node *back;
};
typedef struct QNode *Queue;  //Queue是指標指向QNode

Queue CreateQueue();  //create queue
void AddQ(Queue q, int item);  //將item放入Queue q
int DeleteQ(Queue q);  //將Queue q top 元素刪除並返回
int IsEmpty(Queue q);  //判斷Queue q是否裡面為空

Queue CreateQueue() {  //創造Queue 
  Queue q;  //q 指向 Queue
  q = malloc(sizeof(struct QNode));  //malloc create a Q_node space return pointer 
  q->front = NULL;  //指向第一個的指標為空
  q->back = NULL;  //指向最後一個的指標為空
  return q;  //返回指標q
}
void AddQ(Queue q, int item) {  //將item放入Queue q
  struct Node *new_node;  //指標new_node
  new_node = malloc(sizeof(struct Node));
  new_node->data = item ;  //new_node data為item
  new_node->next = NULL;  //new_node next 為空
  if (q->back == NULL) {  //如果back為空代表該queue為空
    q->front = new_node;  //front 和 back皆為新增的點
    q->back = new_node;
  } else {
    q->back->next = new_node;  //如果不為空，則先使當前back next指向new_node
    q->back = new_node;  //在使back往後一哥變成new_node
  }
}
int  DeleteQ(Queue q) {  //將Queue q top 元素刪除並返回
  struct Node *front_node;  //存還沒進行變動的front以利操作
  int front_value;
  if (q->front == NULL) {  //如果front為空則返回-1
    return 0;
  }
  front_node = q->front;  //如果front不為空，則將該指標賦值給front_node
  if(q->front == q->back){  //如果front = back 代表只有一個元素
    q->front = q->back = NULL;  //使其分別指向空，因為push後就沒元素
  } else {  //如果不只一個元素
    q->front = q->front->next;  //使front移向下一個連接的元素
  }
  front_value = front_node->data;  //front_value儲存還沒移動front時data
  free(front_node);  //歸還空間
  return front_value;  //返回被push元素的值
}
int IsEmpty(Queue q) {
  return (q->front == NULL);  //如果q指向第一個元素為空 ，返回true
}
//-----------------------------
int **graph ;  //graph為二為陣列儲存各節點間可重複走幾次(channel)
int *memory;  //memoey存個節點所含有memory
int *pre;  //在BFS function 中紀錄節點前一點為誰
int **before_shortest_path;  //二為陣列 row id 代表測資id column代表第幾個元素，該陣列存個測資連通所經過點(還沒考慮memory和節點間是否可走)
int *before_shortest_path_size;  //存各測資連通所經過點數量
int **after_shortest_path;  //考慮memory和節點間是否可走，並做相應扣除，最終連通測資儲存在這以利輸出

void Reverse(int count, int id){  //將該陣列元素顛倒
  for(int i = 0 ,j = count; i < j; i++,j--) {
    int temp = before_shortest_path[id][i];
    before_shortest_path[id][i] = before_shortest_path[id][j];
    before_shortest_path[id][j] = temp;
  }
}


void BFS(int start, int end, bool * visit, int id, int node_numbers, int maxium) {  //最短路徑搜索法
  for(int i = 0; i < node_numbers+maxium; i++) {
    pre[i] = -1;  //pre設為-1(-1不可能為節點下標)
    visit[i] = false;  //visit設為false
  }
  bool find = false;  //如果找到end節點 find變為true
  Queue q = CreateQueue();  //創造一個空的queue(先進先出) 名為q
  AddQ(q,start);  //初始添加start 到 q
  visit[start] = true;  //當節點放入queue中 則visit設為true

  while(!IsEmpty(q)) {  //queue裡面如果有節點就不斷循環
    int begin_node = DeleteQ(q);  //將queue裡top彈出，並將其設為當前循環起始點
    if (memory[begin_node] > 0) {  //memory必須大於1才可連接1條以上通道
      for(int i = 0; i < (node_numbers+maxium); i++) {  //從節點0 開始到 節點數+maxium(id從小的開始，解決baseline相同問題)
        if (graph[begin_node][i] != -1) {  //graph不為-1代表有連接(有相鄰)
          if(i == end){  //i為終點
            pre[end]=begin_node;  //該終點前一點為begin_node
            find = true;  //find 為true找到
            break;  //跳出循環
          }
          if (memory[i] >= 2 && graph[begin_node][i] > 0 && !visit[i]) {  //i不為終點 沒有放入queue(拜訪)過 和當前開始點有相鄰 (因為為中間的點必須memory大於2 必定連2條通道)
            AddQ(q,i);  //將該節點放入queue
            visit[i] = true;  //visit(拜訪)設為true
            pre[i] = begin_node;  //該節點前一點為當前循環初始點
          } 
        }
      }
    }
    if (find) break;  //跳出while循環
  }
  if (find) {
    int cur_node = end;  //當前下標為終點
    int count = 0;  //放入元素的序號
    while(cur_node != start) {  //當前節點不為start 就一直循環
      before_shortest_path[id][count] = cur_node;  //id代表測資序號，放入cur_node於column下標為count(從終點開始放)
      cur_node = pre[cur_node];  //cur_node變為該節點的上一個點
      count++;  //放入元素的序號+1
    }
    before_shortest_path[id][count] = start;  //在循環start沒放到，需額外放
    Reverse(count, id);  //將範圍內節點倒過來，使得start先開始到最後end
    before_shortest_path_size[id] = (++count);  //將該測資起點到終點所需經過節點數記錄在before_shortest_path_size
  }
}

void FindSizeSmallest(int reqs, int * temp) {  //找測資內起點到終點所需要經過節點數最小的id
  int cur_id = reqs;  //設置變數存當前最小節點下標，初始設為測資數(因為下標最多到reqs-1，方便比較)
  before_shortest_path_size[cur_id] = INT_MAX;  //該陣列存放 各個測資 最短路徑中會經過節點數量，設置 陣列下標 reqs為整數最大 
  for(int id = 0; id < reqs; id++) {  //循環req次
    if (before_shortest_path_size[id] < before_shortest_path_size[cur_id]) {  //當該id下標 測資最短路徑數量小於 陣列下標cur_id所含有值
      cur_id = id;  //更新cur_id 為當前id
    }
  }
  int size = before_shortest_path_size[cur_id];  //size 為測資中經過最少節點的大小
  before_shortest_path_size[cur_id] = INT_MAX;  //更新 測資中經過最少節點id 其在儲存各測資路徑大小的陣列值為整數最大，避免下次再出現
  temp[0] = cur_id;  //temp[0] 為暫存陣列 儲存當下 測資中經過最少節點的id 
  temp[1] = size;  //temp[1] 為暫存陣列 儲存當下 測資中經過最少節點的大小 
}
bool Check(int size, int id) {  //先檢查該路徑是否能從起始點到終點，如果可以才扣memory和channel
  int start = before_shortest_path[id][0];  //起點為該陣列，第id測資第0個元素
  int end = before_shortest_path[id][size-1];  //終點為該陣列，第id測資第size-1個元素(從0開始)
  if (!memory[start] || !memory[end]) return false;  //如果起點或終點memeory為0則該測資一定不通過，因為無法走到該點
  for(int i = 0; i <= size-2; i++) {  //每次檢查兩筆資料，因此最多循環到終點前一點
    int cur_node = before_shortest_path[id][i];  //cur_id為當前節點
    int next_node = before_shortest_path[id][i+1];  //next_id為當前的下一個節點
    if (i >= 1 && i <= size-2 && memory[cur_node] < 2) return false;  //當節點不為終點或終點時，其memory必須大於2因為其最少連接兩條通道(或內部糾纏)
    if (graph[cur_node][next_node] < 1 && graph[next_node][cur_node] < 1 ) return false;  //節點間的channel必須大於等於1，否則無法連通
  }
  return true;
    
}
void ConnectShortestPath(int reqs) {
  int temp[2];
  FindSizeSmallest(reqs,temp);  //找出所有測資中size最小的測資
  int id = temp[0];  //temp[0]為當前測資id
  int size = temp[1];  //temp[1]為當前測資最短路徑大小
  if (Check(size , id)) {  //check function 判斷該路徑是否能連通，能就進行連接，再將所需memory和channel一一扣除
    for(int i = 0; i <= size-2; i++) {  //循環該測資大小-2，因為每兩筆兩筆看
      int cur_node = before_shortest_path[id][i];  //cur_node為當前節點
      int next_node = before_shortest_path[id][i+1];  //next_node為當前節點下一點
      if (i == 0) {  //為起點
        memory[cur_node]--;  //memory-1
        graph[cur_node][next_node]--;  //之間可走次數-1(channel)
        graph[next_node][cur_node]--;
      } else if (i == size-2) {  //為終點前一點
        memory[cur_node] -= 2;  //該點memory-2
        memory[next_node]--;  //該點下一點為終點，memory-1
        graph[cur_node][next_node]--;  //之間可走次數-1(channel)
        graph[next_node][cur_node]--;
      } else {
        memory[cur_node] -= 2;  //為第二點到終點前兩點
        graph[cur_node][next_node]--;  //之間可走次數-1(channel)
        graph[next_node][cur_node]--;
      }
    }
    for(int i = 0; i < size; i++) {
      after_shortest_path[id][i] = before_shortest_path[id][i];  //將所有結果copy放入輸出結果的陣列
    }
    after_shortest_path[id][size] = -1;  //在存路徑的陣列 終點後一點，使其為-1方便判斷到達終點沒
  } else {
    after_shortest_path[id][0] = -1 ;  //如果無法連通則使該id測資第一個元素為-1
  }
}

void FreeAll(int node_numbers, int reqs, bool *visit, int maxium) {  //將所有申請的空間歸還
  free(memory);
  free(pre);
  free(visit);
  free(before_shortest_path_size);
  for(int i = 0; i < (node_numbers+maxium); i++) {
    free(graph[i]);
  }
  free(graph);
  for(int i = 0; i < reqs; i++) {
    free(before_shortest_path[i]);
  }
  free(before_shortest_path);
  for(int i = 0; i < reqs; i++) {
    free(after_shortest_path[i]);
  }
  free(after_shortest_path);
}

int main(){
  int node_numbers, links, reqs, id;  // node_numbers總共節點數，邊數，測資數，測資id或節點id
  scanf("%d %d %d", &node_numbers, &links, &reqs);  //讀入 總共節點數，link數，測資數
   
  int maxium = 0;  //最大節點id，節點id不一定從0開始
  memory = malloc(sizeof(int) * (node_numbers+1000));  //假定id為1000以內數
  for(int i = 0; i < (node_numbers); i++) {
    scanf("%d", &id); //讀入各id，和將其memory放入對應的陣列memory下標id 
    scanf("%d", &memory[id]);
    if(id > maxium) {
      maxium = id;  //使maxium為最大id數值
    }
  }
                                                  //node_numbers+maxium因為id為maxium裡隨機數(+node_numbers方便後面操作)
  pre = malloc(sizeof(int) * (node_numbers+maxium));
  graph = malloc(sizeof(int*) * (node_numbers+maxium));
  bool *visit = malloc(sizeof(bool) * (node_numbers+maxium));
  before_shortest_path = malloc(sizeof(int*) * (reqs));  //最多有測資數量的資料(row數量為 reqs)
  before_shortest_path_size = malloc(sizeof(int) * (reqs+1));  //+1因為有利後續比較測資連通中size最小
  after_shortest_path = malloc(sizeof(int*) * reqs);  //最多輸出測資數筆資料(row數量為 reqs)

  for(int i = 0; i < (node_numbers+maxium); i++) {
    graph[i]=malloc(sizeof(int) * (node_numbers+maxium));  //創造一個大小為(節點數+maxium)*(節點數+maxium)的二維陣列
  }

  for(int i = 0; i < (node_numbers+maxium); i++) {
    for(int j = 0; j < (node_numbers+maxium); j++) {
      graph[i][j] = -1;  //將graph[link1][link2]初始為-1，如果後續有連接則為channel數
    }
  }
  for(int i = 0; i < reqs; i++) {
    before_shortest_path[i] = malloc(sizeof(int) * node_numbers);  //紀錄沒考慮entaglement swapping時的路徑 
    after_shortest_path[i] = malloc(sizeof(int) * node_numbers);  //紀錄entaglement swapping後的路徑，如果不能重起點到終點就不輸出
  }

  int linkend1,linkend2,channels;  //linkend1、linkend2為相連的節點，之間可重複走channels次
  for(int i = 0; i < links; i++) {
    scanf("%*d %d %d %d", &linkend1, &linkend2, &channels);
    graph[linkend1][linkend2] = channels;  //二維陣列graph，row下標代表節點1，column下標代表節點2，之間可重複走channels次，
    graph[linkend2][linkend1] = channels;  //因為沒方向性，所以也要使 陣列graph 節點2到節點1的可重複走的次數為channels
  }
  
  for(int i = 0; i < reqs; i++) {  //重複reqs次
    int reqsrc, reqdst;
    scanf("%d %d %d",&id ,&reqsrc ,&reqdst);
    BFS(reqsrc,reqdst,visit,id,node_numbers,maxium);  //每讀入一筆測資，就找出其最短路徑(所經過節點數最小)，並存入before_shortest_path[id]陣列中
  } 
  for(int i = 0; i < reqs; i++) {  //重複reqs次
    ConnectShortestPath(reqs);  //找出可使得最多測資，進行糾纏的after_shortest_path[id]
  }  
  int success_number = 0;  //總共成功success_number筆測資，其可行糾纏
  for(id = 0; id < reqs; id++) {  //id代表測資的序號 
    if(after_shortest_path[id][0] == -1){  //如果該筆測資無法從起點到終點就使得該row第一個元素為-1(中間有路斷)
      continue;  //跳過接下來語句
    } else {
      success_number++;  //使得通過測資數加1
    }
  }
  printf("%d\n",success_number);  //先打印總共通過測資數量
  for(id = 0; id < reqs; id++) {  //依照id順序依次打印
    if (after_shortest_path[id][0] == -1) {  //如果該id row 第一個元素為-1代表該筆測資無法從起點到終點
      continue;  //跳過接下來語句，也就是不打印
    } else {
      printf("%d", id);  //先打印成功測資序號(id)              
                                                        //每個after_shortest_path[id]大小為總共節點數
      for(int j = 0; after_shortest_path[id][j] != -1; j++) {  //after_shortest_path[id][j]=-1代表已打印終點
        printf(" %d",after_shortest_path[id][j]);  //打印從起點到終點會經過的點、或糾纏的點
      }
      printf("\n");//換行
    }
  }
  FreeAll(node_numbers,reqs,visit,maxium);  //歸還所有申請的空間
  return 0;
}