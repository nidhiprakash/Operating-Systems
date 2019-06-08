class Person{
private:
	int priority;
	int seq_number;
	int no_of_clothes;
	int laundrytime;

public:
	Person();

//getters and setters for all instance variables of the Person Class


	int getPriority()
	{
		return priority;
	}

	void setPriority(int n)
	{
		priority = n;
	}

	int getSequencenumb()
	{
		return seq_number;
	}

	void setSequencenumb(int sn)
	{
		seq_number = sn;
	}

	int getNumberOfClothes()
	{
		return no_of_clothes;
	}
	void setNumberOfClothes(int nc)
	{
		no_of_clothes = nc;
	}
	int getLaundryTime()
	{
		return laundrytime;
	}
	void setLaundryTime(int lt)
	{
		laundrytime = lt;
	}

	bool operator==(Person *abc) {
			if(abc->getPriority() == this->getPriority() )
				return true;
			else
				return false;
		}

};

Person::Person(){
	//constructor
}