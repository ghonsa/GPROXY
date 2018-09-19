#ifndef GList_Included
#define GList_Included
template <class T, class E> class CGListElement
{
public:
	T * Item;
	E * Next;
	E * Prev;
	CGListElement()
	{
		Item=NULL;
		Next=NULL;
		Prev=NULL;
	};
	~CGListElement()
	{
		delete (T*)Item;
	
	}

	T* GetItem(){ return(Item);};
	void SetItem( T*pItem)
	{
		Item=pItem;
	}
};

template <class T,class E> class CGList
{
public:
	CGList(){
		 NumItems=0;
		 Head=NULL;
		 Tail=NULL;
		 Current=NULL;
	}
	~CGList()
	{
		while(RemoveTail());
	}
	void ResetPtr()
	{
		Current=Head;
	};
	E * GetHead()
	{
		if(Head) return Head->GetItem();
		else return NULL;
	};
	E * GetTail()
	{
		if(!Tail) return NULL;
		return(Tail->GetItem());	
	};
	E * GetCurrent()
	{
		if(!Current) return NULL;
		E* PtrItem=Current->GetItem();	
		return PtrItem;
	};
	E* GetPrev()
	{
		if(!Current) return NULL;
		E* PtrItem=Current->GetItem();	
		Current=Current->Prev;
		return PtrItem;
	};

	E* GetNext()
	{
		if(!Current) return NULL;
		E* PtrItem=Current->GetItem();	
		Current=Current->Next;
		return PtrItem;
	};
	int GetCount()
	{
		return(NumItems);
	};
	void AddToTail(E* Item)
	{
		T* Element= new T;	
		Element->Item=Item;
		Element->Prev=Tail;
		if(!Head) Head=Element;
		if (Tail)Tail->Next=Element;
		Tail=Element;
		NumItems++;
		Current=Element;

	};
	T* RemoveTail()
	{
		
		T * ctail=Tail;
		if(!ctail) return ctail;
		Tail=Tail->Prev;
		Current=Tail;
		//delete(ctail->Item);
		delete(ctail);
		return Tail;
		NumItems--;
	}

private:
	int NumItems;
	T * Head;
	T * Tail;
	T * Current;
};

#endif