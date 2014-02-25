#include <VSslClient>
#include <VSslServer>

class MyServer : public VSslServer
{
  Q_OBJECT

public:
  MyServer(void* owner);

public slots:
  void run___(VSslSession *sslSession);
};
