#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct node* node_pointer;
struct node {  
  struct node *left, *right;  //left and right add and hash to produce parent hash value
  int id;  //only leaf node id is useful
  unsigned long data;  //hash value
};

char* str[10001];  //save all string
unsigned long *hash;  //save all stringconvert to hash value
node_pointer array[10001];  //use to help caculate how many node in each layer
node_pointer MerkleTree(int turn);

unsigned long MurmurOAAT32 (char * key) {
  unsigned long h = 3323198485ul;
  for (; *key; ++key) {
    h ^= *key;
    h *= 0x5bd1e995;
    h ^= h >> 15;
  }
  return h;
}

node_pointer new_a_node(unsigned long num) {  //make a new node right and left point to null and data is hash value
    struct node *new_node;
    new_node = malloc(sizeof(struct node));
    new_node->data = num;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

// Build the bottom 2 layer
node_pointer init_Merkle_tree(int total) {
  unsigned long temp = 0;
  int turn = 0;  //turn represent this layer have how many nodes

  for(int id = 0; id < total - 1; id += 2) {  
    char buffer [sizeof(unsigned long) * 5 + 1];
    temp = hash[id] + hash[id+1];  
    snprintf(buffer, sizeof(buffer), "%lu", temp);  // makes usigned long temp to string(buffer save)
    temp = MurmurOAAT32(buffer);  //temp is hash value how many nodes in this layer
    
    array[turn] = new_a_node(temp);  // make a new node and its hash value is temp
    node_pointer left_node = new_a_node(hash[id]);  // make left child(string convert to hash value derectly)
    left_node->id = id;  //include id which help to find the stings
    node_pointer right_node = new_a_node(hash[id+1]);
    right_node->id = id + 1;
    array[turn]->left = left_node;  //make left and right of new node point to which provide hash value 
    array[turn]->right = right_node;
    turn++;
  }
  if(total % 2 == 1 && total != 1) {  //if have no right silbing , use itself as right to compute 
    char buffer [sizeof(unsigned long) * 5 + 1];
    temp = hash[total-1] + hash[total-1];
    snprintf(buffer, sizeof(buffer), "%lu", temp);
    temp = MurmurOAAT32(buffer);

    array[turn] = new_a_node(temp);
    node_pointer left_node = new_a_node(hash[total-1]);
    left_node->id = total - 1; 
    array[turn]->left = left_node;  //no need to produce a node to right we can only use the left hash value  
    turn++;
  }
  if(total == 1) {  //if only have one string , direct return node include its hash value
    array[0] = new_a_node(hash[0]);
    array[0]->id = 0;
    return array[0];
  }
  return MerkleTree(turn);  //then we build the rest of merkle tree
}

node_pointer MerkleTree(int turn) { 
  node_pointer node_temp = malloc(sizeof(struct node));
  while(turn != 1) {  //when turn == 1 represent we arrive root of tree
    int count = 0;  // the id of node appear in this layer
    unsigned long temp = 0;
    for(int i = 0; i < turn - 1; i += 2) {  //like the above every two hash value of nodes to caculate
      char buffer [sizeof(unsigned long) * 5 + 1];
      temp = array[i]->data + array[i+1]->data;
      snprintf(buffer,sizeof(buffer), "%lu", temp);
      temp = MurmurOAAT32(buffer);

      node_temp = array[i];  //avoid 0th node be cover by new node
      array[count] = new_a_node(temp);
      array[count]->left = node_temp;
      array[count]->right = array[i+1];
      count++;
    }
    if(turn % 2 == 1 && turn != 1) {  //have no siblings, use left to caculate right 
      char buffer [sizeof(unsigned long) * 5 + 1];
      temp = array[turn-1]->data + array[turn-1]->data;
      
      snprintf(buffer,sizeof(buffer), "%lu", temp);
      temp = MurmurOAAT32(buffer);

      node_temp = array[turn-1];
      array[count] = new_a_node(temp);
      array[count]->left = node_temp;
      count++;
    }
    turn = count;  //make turn refresh to the numer of nodes in this new layer
  }
  return array[0];  //return root
}

int wrong[10001];  //save wrong string id 
int times = 0;  //how many wrong string
int Traversal(node_pointer cur, int level, int node_index) {  //inorder traversal
  if(cur == NULL) return -1;  //cur is the current node 
  printf("%d %d %d\n", 1, level, node_index);
  fflush(NULL);
  unsigned long return_num = 0;
  scanf("%lu", &return_num);
  if(cur->data == return_num) return 1;  //if the hash value of node is same as master return right away 
  else{  //else represent there is string is wrong
    int left_result = Traversal(cur->left, level + 1, 2 * node_index);  //find its left child
    int right_result = Traversal(cur->right, level + 1, 2 * node_index + 1);  //find its right child
    if(left_result == -1 && right_result == -1) {  //leaf node only if left and right return -1 
      wrong[times++] = cur->id;  //save the id of leaf node (id help to find wrong string) 
    } 
  }
  return 0;  //other conditons like left is 1, right is null...
}
void print() {
  printf("%d %d\n", 2, times);
  for(int i = 0; i < times; i++) {
    printf("%s\n", str[wrong[i]]);
  }
}
void FreeAll() {
  free(hash);
  for(int i = 0; i < 10001; i++) {
    free(str[i]);
    free(array[i]);
  }
}

int main() {
  int total, maxlen;
  scanf("%d%d", &total, &maxlen);
  hash = malloc(sizeof(unsigned long) * total);  //make all string convert to hash value
  for(int i = 0; i < total; i++) {
    hash[i] = 0;  //init hash
  }

  char *string;  //this temp help to scanf string
  for(int id = 0; id < total; id++) {
    string = malloc(sizeof(char) * maxlen);  //string is pointer
    scanf("%s", string);
    str[id] = string;
  }
  for(int id = 0; id < total; id++) {
    hash[id] = MurmurOAAT32(str[id]);
  }

  node_pointer node_temp = malloc(sizeof(struct node));
  node_temp = init_Merkle_tree(total);
  Traversal(node_temp, 0, 0);
  print();
  FreeAll();
  return 0;
}