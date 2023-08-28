extern "C" {
#include <shoveler/client_property_manager.h>
}

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::UnorderedElementsAre;

namespace {

const int64_t testClientId1 = INT64_MIN;
const int64_t testClientId2 = INT64_MAX;
const long long int testEntityId1 = 1;
const long long int testEntityId2 = 2;
const long long int testEntityId3 = 42;
const char* testComponentTypeId1 = "component_type_1";
const char* testComponentTypeId2 = "component_type_2";

class ShovelerClientPropertyManagerTest : public ::testing::Test {
public:
  virtual void SetUp() {
    clientPropertyManager = shovelerClientPropertyManagerCreate();
    clientIds = g_array_new(/* zeroTerminated */ false, /* clear */ false, sizeof(int64_t));
  }

  virtual void TearDown() {
    shovelerClientPropertyManagerFree(clientPropertyManager);
    g_array_free(clientIds, /* freeSegment */ true);
  }

  std::vector<const char*> GetClientAuthority(int64_t clientId, long long int entityId) const {
    std::vector<const char*> result;
    GArray* array = g_array_new(/* zeroTerminated */ false, /* clear */ false, sizeof(const char*));
    shovelerClientPropertyManagerGetClientAuthority(
        clientPropertyManager, clientId, entityId, array);

    for (guint i = 0; i < array->len; i++) {
      result.push_back(g_array_index(array, const char*, i));
    }

    g_array_free(array, /* freeSegment */ true);
    return result;
  }

  std::vector<const char*> GetClientDeactivations(int64_t clientId, long long int entityId) const {
    std::vector<const char*> result;

    auto* clientDeactivations = shovelerClientPropertyManagerGetClientDeactivations(
        clientPropertyManager, clientId, entityId);
    if (clientDeactivations == NULL) {
      return result;
    }

    GArray* array = g_array_new(/* zeroTerminated */ false, /* clear */ false, sizeof(const char*));
    shovelerClientPropertyManagerClientDeactivationsGetAll(clientDeactivations, array);

    for (guint i = 0; i < array->len; i++) {
      result.push_back(g_array_index(array, const char*, i));
    }

    g_array_free(array, /* freeSegment */ true);
    return result;
  }

  bool GetComponentDeactivation(
      int64_t clientId, long long int entityId, const char* componentTypeId) const {
    return shovelerClientPropertyManagerClientDeactivationsGet(
        shovelerClientPropertyManagerGetClientDeactivations(
            clientPropertyManager, clientId, entityId),
        componentTypeId);
  }

  std::vector<int64_t> ClientIdsVector() const {
    std::vector<int64_t> result(clientIds->len, 0);
    for (int i = 0; i < (int) clientIds->len; i++) {
      result[i] = g_array_index(clientIds, int64_t, i);
    }
    return result;
  }

  ShovelerClientPropertyManager* clientPropertyManager;
  GArray* clientIds;
};

} // namespace

TEST_F(ShovelerClientPropertyManagerTest, addRemoveClient) {
  bool added = shovelerClientPropertyManagerAddClient(clientPropertyManager, testClientId1);
  ASSERT_TRUE(added);
  bool addedAgain = shovelerClientPropertyManagerAddClient(clientPropertyManager, testClientId1);
  ASSERT_FALSE(addedAgain);

  bool removedNonAdded =
      shovelerClientPropertyManagerRemoveClient(clientPropertyManager, testClientId2);
  ASSERT_FALSE(removedNonAdded);
  bool removed = shovelerClientPropertyManagerRemoveClient(clientPropertyManager, testClientId1);
  ASSERT_TRUE(removed);
}

TEST_F(ShovelerClientPropertyManagerTest, clientInterest) {
  shovelerClientPropertyManagerAddClient(clientPropertyManager, testClientId1);

  bool added = shovelerClientPropertyManagerAddEntityInterest(
      clientPropertyManager, testClientId1, testEntityId1);
  ASSERT_TRUE(added);
  bool addedAgain = shovelerClientPropertyManagerAddEntityInterest(
      clientPropertyManager, testClientId1, testEntityId1);
  ASSERT_FALSE(addedAgain);
  ASSERT_TRUE(shovelerClientPropertyManagerHasEntityInterest(
      clientPropertyManager, testClientId1, testEntityId1));
  bool addedNonExisting = shovelerClientPropertyManagerAddEntityInterest(
      clientPropertyManager, testClientId2, testEntityId1);
  ASSERT_FALSE(addedNonExisting);
  ASSERT_FALSE(shovelerClientPropertyManagerHasEntityInterest(
      clientPropertyManager, testClientId2, testEntityId1));

  bool removedNonAdded = shovelerClientPropertyManagerRemoveEntityInterest(
      clientPropertyManager, testClientId2, testEntityId1);
  ASSERT_FALSE(removedNonAdded);
  bool removed = shovelerClientPropertyManagerRemoveEntityInterest(
      clientPropertyManager, testClientId1, testEntityId1);
  ASSERT_TRUE(removed);
  ASSERT_FALSE(shovelerClientPropertyManagerHasEntityInterest(
      clientPropertyManager, testClientId1, testEntityId1));
}

TEST_F(ShovelerClientPropertyManagerTest, entityInterest) {
  shovelerClientPropertyManagerAddClient(clientPropertyManager, testClientId1);
  shovelerClientPropertyManagerAddClient(clientPropertyManager, testClientId2);
  shovelerClientPropertyManagerAddEntityInterest(
      clientPropertyManager, testClientId1, testEntityId1);
  shovelerClientPropertyManagerAddEntityInterest(
      clientPropertyManager, testClientId1, testEntityId2);
  shovelerClientPropertyManagerAddEntityInterest(
      clientPropertyManager, testClientId2, testEntityId1);

  shovelerClientPropertyManagerGetEntityInterest(
      clientPropertyManager, testEntityId1, /* componentTypeId */ nullptr, clientIds);
  ASSERT_THAT(ClientIdsVector(), UnorderedElementsAre(testClientId1, testClientId2));
  shovelerClientPropertyManagerGetEntityInterest(
      clientPropertyManager, testEntityId2, /* componentTypeId */ nullptr, clientIds);
  ASSERT_THAT(ClientIdsVector(), UnorderedElementsAre(testClientId1));
  shovelerClientPropertyManagerGetEntityInterest(
      clientPropertyManager, testEntityId3, /* componentTypeId */ nullptr, clientIds);
  ASSERT_THAT(ClientIdsVector(), IsEmpty());
}

TEST_F(ShovelerClientPropertyManagerTest, componentAuthority) {
  shovelerClientPropertyManagerAddClient(clientPropertyManager, testClientId1);
  shovelerClientPropertyManagerAddClient(clientPropertyManager, testClientId2);

  ASSERT_FALSE(shovelerClientPropertyManagerHasComponentAuthority(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId1));
  ASSERT_FALSE(shovelerClientPropertyManagerHasComponentAuthority(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId2));
  bool authorityAdded = shovelerClientPropertyManagerAddComponentAuthority(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId1);
  ASSERT_TRUE(authorityAdded);
  ASSERT_TRUE(shovelerClientPropertyManagerHasComponentAuthority(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId1));
  ASSERT_FALSE(shovelerClientPropertyManagerHasComponentAuthority(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId2));
  bool authorityAddedAgain = shovelerClientPropertyManagerAddComponentAuthority(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId1);
  ASSERT_FALSE(authorityAddedAgain);

  bool authorityDoubleAdded = shovelerClientPropertyManagerAddComponentAuthority(
      clientPropertyManager, testClientId2, testEntityId1, testComponentTypeId1);
  ASSERT_FALSE(authorityDoubleAdded);
  bool authorityRemoved = shovelerClientPropertyManagerRemoveComponentAuthority(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId1);
  ASSERT_TRUE(authorityRemoved);
  ASSERT_FALSE(shovelerClientPropertyManagerHasComponentAuthority(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId1));
  bool authorityRemovedAgain = shovelerClientPropertyManagerRemoveComponentAuthority(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId1);
  ASSERT_FALSE(authorityRemovedAgain);

  bool authorityReassigned = shovelerClientPropertyManagerAddComponentAuthority(
      clientPropertyManager, testClientId2, testEntityId1, testComponentTypeId1);
  ASSERT_TRUE(authorityReassigned);
  ASSERT_TRUE(shovelerClientPropertyManagerHasComponentAuthority(
      clientPropertyManager, testClientId2, testEntityId1, testComponentTypeId1));
}

TEST_F(ShovelerClientPropertyManagerTest, entityComponentInterest) {
  shovelerClientPropertyManagerAddClient(clientPropertyManager, testClientId1);
  shovelerClientPropertyManagerAddClient(clientPropertyManager, testClientId2);
  shovelerClientPropertyManagerAddEntityInterest(
      clientPropertyManager, testClientId1, testEntityId1);
  shovelerClientPropertyManagerAddEntityInterest(
      clientPropertyManager, testClientId1, testEntityId2);
  shovelerClientPropertyManagerAddEntityInterest(
      clientPropertyManager, testClientId2, testEntityId1);
  shovelerClientPropertyManagerAddComponentAuthority(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId1);
  shovelerClientPropertyManagerAddComponentAuthority(
      clientPropertyManager, testClientId2, testEntityId1, testComponentTypeId2);
  shovelerClientPropertyManagerAddComponentAuthority(
      clientPropertyManager, testClientId1, testEntityId2, testComponentTypeId1);

  // Validate that entity component interest doesn't include authoritative clients.
  shovelerClientPropertyManagerGetEntityInterest(
      clientPropertyManager, testEntityId1, testComponentTypeId1, clientIds);
  ASSERT_THAT(ClientIdsVector(), UnorderedElementsAre(testClientId2));
  shovelerClientPropertyManagerGetEntityInterest(
      clientPropertyManager, testEntityId1, testComponentTypeId2, clientIds);
  ASSERT_THAT(ClientIdsVector(), UnorderedElementsAre(testClientId1));
  shovelerClientPropertyManagerGetEntityInterest(
      clientPropertyManager, testEntityId2, testComponentTypeId1, clientIds);
  ASSERT_THAT(ClientIdsVector(), IsEmpty());

  // Undelegate a component and make sure component interest now includes both clients again.
  shovelerClientPropertyManagerRemoveComponentAuthority(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId1);
  shovelerClientPropertyManagerGetEntityInterest(
      clientPropertyManager, testEntityId1, testComponentTypeId1, clientIds);
  ASSERT_THAT(ClientIdsVector(), UnorderedElementsAre(testClientId1, testClientId2));
}

TEST_F(ShovelerClientPropertyManagerTest, clientAuthority) {
  shovelerClientPropertyManagerAddClient(clientPropertyManager, testClientId1);
  shovelerClientPropertyManagerAddComponentAuthority(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId1);
  shovelerClientPropertyManagerAddComponentAuthority(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId2);
  shovelerClientPropertyManagerAddComponentAuthority(
      clientPropertyManager, testClientId1, testEntityId2, testComponentTypeId1);
  ASSERT_THAT(
      GetClientAuthority(testClientId1, testEntityId1),
      UnorderedElementsAre(testComponentTypeId1, testComponentTypeId2));
  ASSERT_THAT(GetClientAuthority(testClientId1, testEntityId2), ElementsAre(testComponentTypeId1));
  ASSERT_THAT(GetClientAuthority(testClientId1, testEntityId3), IsEmpty());
}

TEST_F(ShovelerClientPropertyManagerTest, componentDeactivation) {
  shovelerClientPropertyManagerAddClient(clientPropertyManager, testClientId1);
  shovelerClientPropertyManagerAddClient(clientPropertyManager, testClientId2);

  ASSERT_FALSE(GetComponentDeactivation(testClientId1, testEntityId1, testComponentTypeId1));
  ASSERT_FALSE(GetComponentDeactivation(testClientId1, testEntityId1, testComponentTypeId2));
  bool activationAdded = shovelerClientPropertyManagerAddComponentDeactivation(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId1);
  ASSERT_TRUE(activationAdded);
  ASSERT_TRUE(GetComponentDeactivation(testClientId1, testEntityId1, testComponentTypeId1));
  ASSERT_FALSE(GetComponentDeactivation(testClientId1, testEntityId1, testComponentTypeId2));
  bool activationAddedAgain = shovelerClientPropertyManagerAddComponentDeactivation(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId1);
  ASSERT_FALSE(activationAddedAgain);

  bool activationDoubleAdded = shovelerClientPropertyManagerAddComponentDeactivation(
      clientPropertyManager, testClientId2, testEntityId1, testComponentTypeId1);
  ASSERT_TRUE(activationDoubleAdded);
  ASSERT_TRUE(GetComponentDeactivation(testClientId1, testEntityId1, testComponentTypeId1));
  ASSERT_TRUE(GetComponentDeactivation(testClientId2, testEntityId1, testComponentTypeId1));

  bool activationRemoved = shovelerClientPropertyManagerRemoveComponentDeactivation(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId1);
  ASSERT_TRUE(activationRemoved);
  ASSERT_FALSE(GetComponentDeactivation(testClientId1, testEntityId1, testComponentTypeId1));
  ASSERT_TRUE(GetComponentDeactivation(testClientId2, testEntityId1, testComponentTypeId1));
  bool activationRemovedAgain = shovelerClientPropertyManagerRemoveComponentAuthority(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId1);
  ASSERT_FALSE(activationRemovedAgain);
}

TEST_F(ShovelerClientPropertyManagerTest, clientDectivations) {
  shovelerClientPropertyManagerAddClient(clientPropertyManager, testClientId1);
  shovelerClientPropertyManagerAddClient(clientPropertyManager, testClientId2);

  shovelerClientPropertyManagerAddComponentDeactivation(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId1);
  shovelerClientPropertyManagerAddComponentDeactivation(
      clientPropertyManager, testClientId1, testEntityId1, testComponentTypeId2);
  shovelerClientPropertyManagerAddComponentDeactivation(
      clientPropertyManager, testClientId2, testEntityId1, testComponentTypeId1);
  ASSERT_THAT(
      GetClientDeactivations(testClientId1, testEntityId1),
      UnorderedElementsAre(testComponentTypeId1, testComponentTypeId2));
  ASSERT_THAT(
      GetClientDeactivations(testClientId2, testEntityId1), ElementsAre(testComponentTypeId1));
  ASSERT_THAT(GetClientDeactivations(testClientId1, testEntityId2), IsEmpty());
}