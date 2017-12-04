#include <iostream>
#include <map>
#include <string>


class ManufacturedBase
{
public:
	virtual ~ManufacturedBase() {};
	virtual ManufacturedBase* clone() const = 0;
	virtual void whoami() const = 0;
};


class Factory
{
private:
	typedef std::map<std::string, ManufacturedBase*> FactoryMap;
	FactoryMap m_map;

public:
	virtual ~Factory() {
		FactoryMap::iterator it = m_map.begin();
		for(; it != m_map.end(); ++it) {
			if (it->second) {
				delete it->second;
			}
		}
	}

public:
	void record(const std::string &key, const ManufacturedBase &object)
	{
		if(m_map.find(key) == m_map.end()) {
			m_map[key] = object.clone();
		}
	}

	ManufacturedBase* create(const std::string &key) const
	{
		FactoryMap::const_iterator it = m_map.find(key);
		if(it != m_map.end()) {
			return it->second->clone();
		}
		else {
			return nullptr;
		}
	}

};



class Car: public ManufacturedBase {
	virtual ManufacturedBase* clone() const
	{
		return new Car;
	}

	virtual void whoami() const
	{
		std::cout << "Car" << std::endl;
	}
};

class Truck: public ManufacturedBase {
	virtual ManufacturedBase* clone() const
	{
		return new Truck;
	}

	virtual void whoami() const
	{
		std::cout << "truck" << std::endl;
	}
};


int main(void)
{
	Factory factory;

	factory.record("Truck", Truck());
	factory.record("Car", Car());

	ManufacturedBase *a = factory.create("Car");
	ManufacturedBase *b = factory.create("Truck");

	b->whoami();
	a->whoami();

	delete a;
	delete b;

	return 0;
}
