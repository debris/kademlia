
#pragma once
#include <vector>
#include <array>
#include <algorithm>
#include <memory>

namespace kademlia
{

static const unsigned nodeLength = 20;
	
using byte = uint8_t;
using NodeID = std::array<byte, nodeLength>;

static const unsigned bucketSize = 20;

struct Contact
{
	NodeID node;
};
	
struct Bucket
{
	byte depth;
	// contacts are ordered from the oldest
	std::vector<Contact> contacts;
	std::shared_ptr<Bucket> parent;
	std::shared_ptr<Bucket> left;
	std::shared_ptr<Bucket> right;
};

	
class RoutingTable
{
public:
	
	void addContact(Contact const& _contact)
	{
		auto bucket = getBucket(_contact.node);
		addContact(_contact, bucket);
	}
	
	void addContact(Contact const& _contact, std::shared_ptr<Bucket> const& _bucket)
	{
		auto contacts = _bucket->contacts;
		auto it = std::find_if(contacts.begin(), contacts.end(), [_contact](Contact const& current)
		{
			return current.node == _contact.node;
		});
		
		if (it != contacts.end())
		{
			contacts.erase(it);
			contacts.push_back(_contact); // recent contacts are always at the end
		}
		else
		{
			// check if there is enought place to
			if (contacts.size() < bucketSize)
				contacts.push_back(_contact);
			else if (_bucket->depth < 5) // we shouldn't go deeper than 5
			{
				// split the bucket to make some place for new _contact
				splitBucket(_bucket);
				
				// get one of the buckets that we just created
				auto childBucket = getBucket(_contact.node, _bucket);
				
				// go through all of this stuff again, cause all contacts might have the same prefix (rare, but...)
				addContact(_contact, childBucket);
			}
			else if (pingContact(contacts.front())) // send ping to the first guy in contacts if we are on level 5
			{
				// move the guy to the and of vector if he responded
				
				// TODO: rotate
				auto pinged = contacts.front();
				contacts.erase(contacts.begin());
				contacts.push_back(pinged);
				
				// sry, there is no place for new _contact, we drop that
			}
			else
			{
				// guy didnt respond for ping, drop him
				contacts.erase(contacts.begin());
				
				// push new contact instead
				contacts.push_back(_contact);
			}
		}
	}
	
	std::shared_ptr<Bucket> getBucket(NodeID const& _node)
	{
		return getBucket(_node, m_bucket);
	}
	
	std::shared_ptr<Bucket> getBucket(NodeID const& _node, std::shared_ptr<Bucket> const& _bucket)
	{
		if (_bucket->left || _bucket->right)
		{	// both of them should always be initialized
			if (((_node[0] >> byte(7 - _bucket->depth)) & 0x1) == 0)
				return getBucket(_node, _bucket->left);
			return getBucket(_node, _bucket->right);
		}
		return _bucket;
	}
	
	void splitBucket(std::shared_ptr<Bucket> const& _bucket)
	{
		_bucket->left.reset(new Bucket);
		_bucket->right.reset(new Bucket);
		_bucket->left->depth = _bucket->depth + 1;
		_bucket->right->depth = _bucket->depth + 1;
		auto contacts = _bucket->contacts;
		
		for (auto _contact: contacts)
			if (((_contact.node[0] >> byte(7 - _bucket->depth)) & 0x1) == 0)
				_bucket->left->contacts.push_back(_contact);
			else
				_bucket->right->contacts.push_back(_contact);
		contacts.clear();
	}
	
	std::vector<Contact> getClosestContacts(Contact const& _contact)
	{
		auto bucket = getBucket(_contact.node);
		// TODO: we may want to optimize that, use XOR or whatever
		return bucket->contacts;
	}
	
	// returns true if he is alive
	bool pingContact(Contact const& _contact)
	{
		// TODO implement pinging
		return true;
	}
	
private:
	NodeID m_node;
	std::shared_ptr<Bucket> m_bucket;
};

}