Hi everybody! Actually, I am not a master of working with graphs, but having found a task on one of the C++ forums, I became interested and decided to stretch my muscles) 
Unfortunately, I did not find working examples of the implementation of either the Dijkstra's algorithm or the Belman-Ford algorithms, but even the non-working examples seemed ineffective to me at first glance. 
That is why I decided to create my own algorithm. It is closely related to the choice of data structures. Several rather fruitful ideas came to mind one after another. Motivation:
1. Use a common storage for graph nodes - a set of nodes. This allows you to save a chain of visitors without creating heavy vectors for storing paths.
2. Use a hash map to store vectors of available nodes from the node - the key. This also dramatically speeds up the algorithm.
3. To ensure points 1 and 2, wrap the indices - the numbers of graph nodes in a node class, where you can store a pointer to the last visitor.
4. Wrap the node according to item 3 with a class pnode, storing a pointer to the node and conducting a hash function of the node class hash function of the node class.
Everything is not very concise in the code, but it works very quickly and is not afraid of even very large graphs in terms of the number of their nodes.
Do not look on how ugly it looks using class Solution and other thing... That was the leetcode condition. There are many strange things there like already given the structure of flights, number of cities etc. I was forced to make my code to be related to their demands.
