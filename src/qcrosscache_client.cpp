#include "./qcrosscache_client.h"
#include "./qcrosscache_actuator_interface.h"
#include "./qcrosscache_actuator_manager.h"
#include "./qcrosscache_server.h"
#include <QVariantHash>
#include <QVariantList>

namespace QCrossCache {

#define dPvt() auto &p = *reinterpret_cast<ClientPvt *>(this->p)

class ClientPvt
{
public:
    Client *parent = nullptr;
    ActuatorInterface *interface = nullptr;

    explicit ClientPvt(Client *parent, ActuatorInterface *interface)
    {
        this->parent = parent;
        this->interface = interface;
        if (this->interface != nullptr)
            this->interface->setParent(parent);
    }

    virtual ~ClientPvt()
    {
        if (this->interface != nullptr)
            delete this->interface;
    }
};

Client::Client(QObject *parent) : QObject{parent}
{
    auto acLocal = ActuatorManager::instance().interfaceCreate(QByteArrayLiteral("local"));
    this->p = new ClientPvt(this, acLocal);
}

Client::Client(const QByteArray &dataGroup, QObject *parent) : QObject{parent}
{
    Q_UNUSED(dataGroup);
    auto acLocal = ActuatorManager::instance().interfaceCreate(QByteArrayLiteral("local"));
    acLocal->setDataGroup(dataGroup);
    this->p = new ClientPvt(this, acLocal);
}

Client::Client(ActuatorInterface *interface, QObject *parent) : QObject{parent}
{
    this->p = new ClientPvt(this, interface);
}

Client::~Client()
{
    dPvt();
    delete &p;
}

Server *Client::server()
{
    dPvt();
    if (p.interface == nullptr)
        return nullptr;
    return p.interface->server();
}

const QByteArray &Client::dataGroup() const
{
    dPvt();
    if (p.interface == nullptr) {
        static QByteArray dataGroup;
        return dataGroup;
    }
    return p.interface->dataGroup();
}

bool Client::connect()
{
    dPvt();
    if (p.interface == nullptr)
        return false;
    return p.interface->connect();
}

bool Client::disconnect()
{
    dPvt();
    if (p.interface == nullptr)
        return false;
    return p.interface->disconnect();
}

bool Client::isConnected()
{
    dPvt();
    if (p.interface == nullptr)
        return false;
    return p.interface->isConnected();
}

bool Client::clear()
{
    dPvt();
    if (p.interface == nullptr)
        return false;
    return p.interface->clear();
}

bool Client::exists(const QByteArray &key)
{
    dPvt();
    if (p.interface == nullptr)
        return false;
    return p.interface->exists(key);
}

bool Client::put(const QByteArray &key, const QByteArray &data, const quint64 expiration)
{
    dPvt();
    if (p.interface == nullptr)
        return false;
    return p.interface->put(key, data, expiration);
}

QByteArray Client::get(const QByteArray &key)
{
    dPvt();
    if (p.interface == nullptr)
        return {};
    return p.interface->get(key);
}

QByteArray Client::take(const QByteArray &key)
{
    dPvt();
    if (p.interface == nullptr)
        return {};
    return p.interface->take(key);
}

bool Client::remove(const QByteArray &key)
{
    dPvt();
    if (p.interface == nullptr)
        return false;
    return p.interface->remove(key);
}

QVector<QByteArray> Client::list(const QByteArray &key)
{
    dPvt();
    if (p.interface == nullptr)
        return {};
    return p.interface->list(key);
}

QVector<QByteArray> Client::listKeys()
{
    dPvt();
    if (p.interface == nullptr)
        return {};
    return p.interface->listKeys();
}

Client *clientForLocal(QObject *parent)
{
    auto interface = ActuatorManager::instance().interfaceCreate(QByteArrayLiteral("local"));
    if (interface == nullptr)
        return nullptr;

    return new Client(interface, parent);
}

Client *clientForMemcached(QObject *parent)
{
    auto interface = ActuatorManager::instance().interfaceCreate(QByteArrayLiteral("mancached"));
    if (interface == nullptr)
        return nullptr;

    return new Client(interface, parent);
}

Client *clientForMongoDb(QObject *parent)
{
    auto interface = ActuatorManager::instance().interfaceCreate(QByteArrayLiteral("mongodb"));
    if (interface == nullptr)
        return nullptr;
    return new Client(interface, parent);
}

Client *clientForRedis(QObject *parent)
{
    auto interface = ActuatorManager::instance().interfaceCreate(QByteArrayLiteral("redis"));
    if (interface == nullptr)
        return nullptr;
    return new Client(interface, parent);
}

} // namespace QCrossCache
