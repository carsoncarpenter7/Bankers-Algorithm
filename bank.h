//  bank.h

#ifndef __BANK_H__
#define __BANK_H__

#include "customer.h"
// TODO
class Bank
{
public:
    Bank() = default;
    Bank(const ext_vector<int>& available) : avail(available), customers() {}

    ext_vector<int> get_avail() const { return avail; }
    bool is_avail(const ext_vector<int>& req) const { return req < avail; }

    // Check if alloc is safe
    bool is_safe(int id, const ext_vector<int>& req)
    {
        bool minimum = false;

        // create copy of bank available resources
        ext_vector<int> bank_temp = this->avail;
        bank_temp -= req;

        // satifies another request?
        for (size_t x = 0; x < this->avail.size(); ++x)
        {
            if (x == id)
                continue;
            if (bank_temp >= this->customers[x]->create_req())
            {
                minimum = true;
            }
        }

        return minimum;
    }

    // Approve Request
    bool req_approved(int id, const ext_vector<int>& req)
    {
        if (req > avail)
        {
            return false;
        }

        const Customer* c = customers[id];
        if (c->needs_exceeded(req))
        {
            return false;
        }

        return is_safe(id, req);
    }

    // Add customer to bank
    void add_customer(Customer* c)
    {
        customers.push_back(c);
        avail -= c->allocated();
    }

    // Error Message 
    void withdraw_resources(const ext_vector<int>& req)
    {
        if (!is_avail(req))
        {
            pthread_mutex_lock(&mutex_);
            std::cerr << "WARNING: req: " << req << " is not available for withdrawing\n";
            pthread_mutex_unlock(&mutex_);
            return;
        }
        if (is_avail(req))
        {
            avail -= req;
        }
    }
    void deposit_resources(const ext_vector<int>& req) { avail += req; }

    ext_vector<Customer*> get_customers() const { return customers; }

    // Print Avalaible
    void show() const
    {
        pthread_mutex_lock(&mutex_);
        std::cout << "available: [" << avail << "]\n";
        pthread_mutex_unlock(&mutex_);

        for (Customer* c : customers)
        {
            c->show();
        }
        std::cout << "\n";
    }

    friend std::ostream& operator<<(std::ostream& os, const Bank& be)
    {
        be.show();
        return os;
    }

private:
    ext_vector<int> avail;
    ext_vector<Customer*> customers;
};

#endif /* Bank_h */