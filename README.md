# AssetTagTreePlugin

Using the AssetTagTreePlugin, it is possible to categorize UObject Assets in Unreal Engine 5, using GameplayTags; retrieve all Assets within a category, as well as parent and child categories; 
and subscribe to categories to stay informed about changes utilizing the [Observer Design Pattern](https://refactoring.guru/design-patterns/observer).

<img width="596" height="361" alt="tag-tree" src="https://github.com/user-attachments/assets/b4570f19-ee13-45f9-8fff-2c4c61b024cf" />

## How does it work?
The main components of this Plugin are
- The FAssetTagTree,
- FAssetTagSubjects,
- AssetTagObservers,
- and the AssetTagTreeSubsystem

### AssetTagTree
The AssetTagTree (ATT) is a tree structure which mimics the structure of all GameplayTags used for categorization, using GameplayTags as identifiers for nodes. 
A node within this tree stores UObject references of subjects, which are directly assigned to this node, and callback delegates which are used to inform observers about changes.

> Note: The AssetTagTree is NOT being persisted. It only lives in-memory, sharing the life cycle of the engine.

### AssetTagTreeSubsystem
The AssetTagTreeSubsystem is an EngineSubsystem used to manage the AssetTagTree and provides functionality to add/remove subjects to/from the ATT, 
request all AssetTagSubjects assigned to certain GameplayTags and manage AssetTagObservers.

![assetTagInsertion_sequenzDiagram (1)](https://github.com/user-attachments/assets/d90138a1-9e72-4bc9-8945-e2c6ec2b95d2)

### AssetTagSubject
AssetTagSubjects (subjects) are used to categorize UObject Assets with GameplayTags. If a tag is being added/removed to the internal gameplaytag container, 
the subject aka. the UObject which should be categorized, is being added/removed to/from the AssetTagTree and all relevant observers are being notified about the change.

### AssetTagObsever
By adding tags to the internal GameplayTagContainer a AssetTagObserver, or to be more specific its callback function, is being added to the corrisponding nodes of the AssetTagTree.
By setting up the ECollectionStrategy enum, subjects from child or parent tags can also be imported. 
