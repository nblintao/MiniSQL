#ifndef _bufferManager_h_
#define _bufferManager_h_

#define BLOCKSIZE 4096
#define BUFFERSIZE 1024
#define HASHSIZE 1000007

#include <list>
#include <string>

using namespace std;

/**
	该文件是MiniSQL中的缓冲区管理模块头文件.
	推荐采取的文件存储方式 : 
		为每一张表建立一个文件.
		为每一个索引建立一个文件
	通过传入文件名到该头文件中定义的函数中的方式来读取某个表/索引中的特定块.
	注意, 读取到的块的开头地址(offset)都是4096的整数倍, 每个块的大小为4KB
**/

struct Block{
	string fileName;
	//which file's block
	unsigned char data[BLOCKSIZE];
	//data part
	long offset;
	//start address
	int status;
	//block status
};

class BFM{
public:
	Block* BufferManagerRead(const string &fileName, long offset);
	/*
		输入：1.需要读取的文件名 2.起始地址(请确保为BLOCKSIZE的倍数)
		操作：首先检查该块是否在主存缓冲区中, 若不在则在缓冲区中添加该块. 
		      如果需要的话, 会按LRU策略从缓冲区中移出其他块来腾出空间.
		输出：指向缓冲区中这个块的指针
	*/
	void BufferManagerPin(Block *b);
	/*
		输入：指向需要锁定的缓冲区块的指针
		操作：修改这个块的状态为: 不允许被写出.
		输出：无
	*/
	void BufferManagerWrite(Block *b);
	/*
		输入：指向需要强制写出的缓冲区块的指针.(可以将通过BufferManagerPin锁定的块写出)
		操作：将这个块的内容写入磁盘中
		输出：无
	*/
	int  BufferManagerGetStatus(Block *b);
	/*
		输入：指向需要获得状态的缓冲区块的指针
		操作：定位对应的块, 读取其状态
		输出：0--普通状态(可能被替换出去)
		      1--锁定状态(不会被替换出去, 可以被强制写出)
	*/
	void flush();
	/*
		将缓冲区中所有的块都强制写出.
	*/
private:
	list <Block> buffer;
	struct HashElement{
		list <Block> :: iterator it;
		int status;
	};
	HashElement hashTable[HASHSIZE];
};

#endif