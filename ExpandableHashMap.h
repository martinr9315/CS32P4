// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.
#include <list>
#include <vector>

//FOR TESTING
#include <iostream>
using namespace std;

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
    ~ExpandableHashMap();
    void reset();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    ExpandableHashMap(const ExpandableHashMap&) = delete;
    ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;
    
    //TESTING
    void print() const;
    
private:
    struct Pair //pair of key, value
    {
        Pair(KeyType k, ValueType v)
        {
            m_key = k;
            m_value = v;
        }
        KeyType m_key; //holds 1 key, whatever that may be
        ValueType m_value; //holds 1 key, whatever that may be
    };
    
    //bucket structure
    struct Bucket
    {
        std::list<Pair> m_PairList;  //holds list of key, value pairs, whatever types those may be
    };
    
    std::vector<Bucket*> bucketV;  //resizable vector of pointers to buckets
    int m_nBuckets;  //number of buckets in map
    int m_nAssoc; //number of associations in map
    double m_loadFactor; //load factor of map
public://REMOVE THIS AFTER TESTING
    unsigned int getBucketNumber(const KeyType& key) const
    {
        unsigned int hasher(const KeyType& k); // prototype
        unsigned int h = hasher(key);
        return h % m_nBuckets;
    }
    
    void clear();
};

template <typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
    m_nBuckets = 8; //set number of buckets to 8
    m_nAssoc = 0; //set number of associations to 0
    m_loadFactor = maximumLoadFactor;
    for (int i=0; i<m_nBuckets; i++)
    {
        bucketV.push_back(new Bucket); //add 8 empty buckets to the buckets vector
    }
}
template <typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
    clear();
}

template <typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
    clear(); //delete everything in current hashmap using destructor
    m_nAssoc = 0; //set associations back to zero
    m_nBuckets = 8; //set number of buckets back to 8
    for (int i=0; i<m_nBuckets; i++)
    {
        bucketV.push_back(new Bucket); //push 8 empty buckets into bucket vector
    }
}

template <typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
    return m_nAssoc; //return number of associations in map
}

template <typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    //check if key is already in map
    ValueType* foundV = find(key);
    if (foundV != nullptr) //if is in map
        *foundV=value; //set associated value to value parameter
    
    else //if not already in map
    {
        //check that load factor is not exceeded
        double currentLoad=static_cast<double>(m_nAssoc+1)/ static_cast<double>(m_nBuckets);
        if (currentLoad>m_loadFactor) //if it is exceeded
        {
            ExpandableHashMap temp(m_loadFactor); //construct new temporary map with target load factor
            temp.m_nBuckets = m_nBuckets*2; //double number of buckets in new map
            for (int i=0; i<temp.m_nBuckets; i++)
            {
                temp.bucketV.push_back(new Bucket); //push empty buckets into bucket vector
            }
            
            //rehash all associations from original map into new map
            for (int i=0; i<m_nBuckets; i++)
            {
                auto it = bucketV[i]->m_PairList.begin(); //loop through each buckets pair list
                while (it != bucketV[i]->m_PairList.end())
                {
                    //get key and rehash
                    KeyType k = (*it).m_key; //get key in current pair
                    ValueType v = (*it).m_value; //get value in current pair
                    int n = temp.getBucketNumber(k); //get bucket number for new map w current key
                    Pair toInsert(k, v);
                    (temp.bucketV[n]->m_PairList).push_back(toInsert);
                    it++;
                }
            }
            clear();//delete all associations in original map
            bucketV.swap(temp.bucketV); //re assign bucket pointer vector to origial map
            m_nBuckets = temp.m_nBuckets; //update number of buckets
        }
        
        //insert new value
        int n = getBucketNumber(key); //get bucket number with key
        Pair newInsert(key, value);
        (bucketV[n]->m_PairList).push_back(newInsert); //push new value to correct buckets value list
        m_nAssoc++;
    }
}

template <typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
    int n = getBucketNumber(key);
    auto it = bucketV[n]->m_PairList.begin(); //loop through each buckets pair list
    while (it != bucketV[n]->m_PairList.end())
    {
        if ((*it).m_key==key) //if key already in map
            return &((*it).m_value); //return pointer to the associated value
        it++;
    }
    return nullptr; //else, return null ptr
}

template <typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::clear()
{
    auto it = bucketV.begin(); //loop through each buckets
    while (it != bucketV.end())
    {
        Bucket* temp = (*it);
        it = bucketV.erase(it);
        delete temp;
    }
}

//FOR TESTING
template <typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::print() const
{
    for (int i=0; i<m_nBuckets; i++)
    {
        cout << "Bucket #"<<i<<endl;
        auto it = bucketV[i]->m_PairList.begin(); //loop through each buckets pair list
        while (it != bucketV[i]->m_PairList.end())
        {
            
            cout <<"key: "<<(*it).m_key.print()<<endl;
            cout <<" values: "<<(*it).m_value.print()<<endl;
            /*auto itV = ((*it).m_value).begin(); //loop through each pair's vector
            while (itV != ((*it).m_value).end())
            {
                cout <<(*itV).print()<<endl;
                itV++;
            }*/
            
            it++;
        }
    }
}
