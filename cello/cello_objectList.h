/*
    Copyright (c) 2024 Ricardo Dias
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "cello_update_source.h"
#include "cello_object.h"

namespace cello
{

/**
 * @class ObjectList
 * @brief A class representing a list of objects derived from the Object class.
 *        It provides functionality for managing and manipulating the objects in the list.
 *
 * The ObjectList class is designed to be used as a base class for creating specific object lists.
 * It provides common functionality such as adding, removing, and accessing objects in the list.
 * Derived classes must implement the abstract methods isValidAsChild() and createNewObject()
 * to define the criteria for object suitability and object creation, respectively.
 *
 * @tparam ObjectType The type of objects stored in the list, derived from the Object class.
 */
template<typename ObjectType, typename CriticalSectionType = juce::DummyCriticalSection>
class ObjectList 
    : public Object
{
public:
    
    /**
     * @class ObjectList
     * @brief Represents a list of objects.
     *
     * This class inherits from the Object class and provides a way to manage a list of objects.
     * It is constructed with a type identifier and a parent ValueTree.
     * 
     * Derived classes should call rebuildObjects() in the construction to populate the list with objects
     * 
     * @param type The type identifier of this list.
     * @param parentTree The parent ValueTree of this list.
     */
    ObjectList (const juce::Identifier& type, juce::ValueTree parentTree)
        : Object { type, parentTree }
    {
    }
    
    /**
     * @class ObjectList
     * @brief Represents a list of objects.
     *
     * The ObjectList class is a subclass of Object and represents a list of objects.
     * It provides a constructor that takes a juce::Identifier and an Object as parameters.
     * The constructor initializes the ObjectList with the given type and parent Object.
     * 
     * Derived classes should call rebuildObjects() in the construction to populate the list with objects
     * 
     * @param type The type identifier of this list.
     * @param parent The parent Object of this list.
     */
    ObjectList (const juce::Identifier& type, const Object& parent)
        : Object (type, parent)
    {
    }

    /**
     * @class ObjectList
     * @brief Represents a list of objects.
     *
     * The ObjectList class is a subclass of Object and represents a list of objects.
     * It provides a constructor that takes a juce::Identifier and an Object as parameters.
     * The constructor initializes the ObjectList with the given type and parent Object.
     * 
     * Derived classes should call rebuildObjects() in the construction to populate the list with objects
     * 
     * @param type The type identifier of this list.
     * @param parent The parent Object of this list.
     */
    ObjectList (const juce::Identifier& type, const Object* parent)
        : Object { type, (parent != nullptr ? static_cast<juce::ValueTree>(*parent) : juce::ValueTree()) }
    {
    }

    
    /**
     * @brief A class representing a list of objects.
     * 
     * This class is derived from the Object class and provides a way to store and manipulate a list of objects.
     * It supports copying of objects using the copy constructor.
     * 
     * Derived classes should call rebuildObjects() in the construction to populate the list with objects
     * 
     * @param rhs The ObjectList to copy from.
     */
    ObjectList (const ObjectList& rhs)
        : Object (rhs)
    {
    }

    /**
     * @brief Destructor for the ObjectList class.
     * 
     * This destructor verifies that the list of objects is empty before destruction.
     * It asserts that the `freeObjects()` function has been called to release all objects.
     */
    virtual ~ObjectList () override
    {
        jassert(_objects.size() == 0); // must call freeObjects() first
    }


    /**
     * @brief Clears the object list, removing all elements.
     */
    void clear()
    {
        while (objectCount() > 0)
            removeObject(getObject(0));
    }

    /**
     * Returns the number of objects in the internal object list.
     *
     * @return The number of objects in the internal object list.
     */
    int objectCount() const
    {
        return _objects.size();
    }

    /**
     * @brief Get the object at the specified index.
     *
     * @param index The index of the object to retrieve.
     * @return A pointer to the object at the specified index, or nullptr if the index is out of range.
     */
    ObjectType* getObject(int index) const
    {
        if (index < 0 || index >= _objects.size())
        {
            jassertfalse;
            return nullptr;
        }

        return _objects[index];
    }

    void removeObject(ObjectType* obj)
    {
        juce::ValueTree parent = *this;
		parent.removeChild(*obj, getUndoManager());
    }

    void removeAllChildren()
    {
        juce::ValueTree parent = *this;
        parent.removeAllChildren(getUndoManager());
    }

    /**
     * Returns a const reference to the juce::OwnedArray<ObjectType> object list.
     *
     * @return A const reference to the object list.
     */
    const juce::OwnedArray<ObjectType>& objectList() const
    {
        return _objects;
    }

    void move(int oldIndex, int newIndex)
    {
        int n = _objects.size();
        if (oldIndex < 0 || oldIndex >= n)
            return;

        if (newIndex < 0 || newIndex >= n)
            return;

        if (oldIndex == newIndex)
            return;

        juce::ValueTree parent = *this;
        parent.moveChild(oldIndex, newIndex, getUndoManager());
    }

    void moveUp(int index)
    {
        if (index < 1 || index >= _objects.size())
            return;

        move(index, index - 1);
    }

    void moveDown(int index)
    {
        if (index < 0 || index >= _objects.size() - 1)
            return;

        move(index, index + 1);
    }

    void moveUp(ObjectType* obj)
    {
        moveUp(_objects.indexOf(obj));
    }

    void moveDown(ObjectType* obj)
    {
        moveDown(_objects.indexOf(obj));
    }

protected:

    /**
     * @brief Checks if the given juce::ValueTree is valid as a child of this list.
     * 
     * This function is a pure virtual function that must be implemented by derived classes.
     * 
     * @param childTree The juce::ValueTree to check.
     * @return true if the valueTree is of a suitable type, false otherwise.
     */
    virtual bool isValidAsChild(const juce::ValueTree& childTree) const = 0;

    /**
     * @brief Creates a new object of the specified ObjectType using the given childTree.
     *
     * This function is a pure virtual function that must be implemented by derived classes.
     *
     * @param childTree The ValueTree representing the child object.
     * @return A pointer to the newly created object.
     */
    virtual ObjectType* createNewObject(const juce::ValueTree& childTree) = 0;

    /**
     * @brief Rebuilds the internal object list from the parent ValueTree.
     *
     * This function should be called by derived classes to populate the list with objects.
     * It should only be called once at construction time.
     */
    void rebuildObjects(bool deleteInvalidChildren = true)
    {
        jassert(_objects.size() == 0); // must only call this method once at construction
        _objects.clear();
        std::vector<int> invalidChildrenIndexes;
        for (const auto& v : *this)
        {
            auto type = v.getType();
            auto typeString = type.toString();
            ObjectType* newObject = nullptr;
            if (isValidAsChild(v))
            {
                newObject = createNewObject(v);
            }

            if (newObject)
                _objects.add(newObject);
            else
            {
                invalidChildrenIndexes.push_back(indexOf(v));
            }
        }

        if (deleteInvalidChildren)
        {
            // Reverse the order of the indexes to remove to avoid invalidating them
            std::sort(invalidChildrenIndexes.begin(), invalidChildrenIndexes.end(), std::greater<>());
            for (int i : invalidChildrenIndexes)
            {
                remove(i);
            }
        }
    }

    /**
     * @brief Frees all objects in the internal object list.
     *
     * This function should be called by derived classes to release all objects in the list.
     * It should only be called once at destruction time.
     */
    void freeObjects()
    {
        _objects.clear();
    }
    
    /**
     * Checks if the given ValueTree is a child tree of the current object.
     *
     * @param v The ValueTree to check.
     * @return True if the given ValueTree is a child tree, false otherwise.
     */
    bool isChildTree(juce::ValueTree& v) const
    {
        return isValidAsChild(v) && v.getParent() == *this;
    }

    // ---
    // Useful callbacks for derived classes

    /**
     * @brief This function is called when an object is added to the object list.
     *
     * @param obj The object that was added.
     */
    virtual void onObjectAdded(ObjectType&) { }

    /**
     * @brief This function is called when an object is removed from the object list.
     *
     * @param obj The object that was removed.
     */
    virtual void onObjectRemoved(ObjectType&) { }

    /**
     * @brief This function is called when the order of objects in the list is changed.
     *
     * @param oldIndex The index of the object before the change.
     * @param newIndex The index of the object after the change.
     */
    virtual void onObjectOrderChanged(int /*oldIndex*/, int /*newIndex*/) { }

private:
    /**
     * @brief Will execute the callback `onObjectAdded` if it exists.
     *
     * @param parentTree
     * @param childTree
     */
    virtual void valueTreeChildAdded (juce::ValueTree& parentTree, juce::ValueTree& childTree) override
    {
        Object::valueTreeChildAdded(parentTree, childTree);

        if (!isChildTree(childTree))
            return;

        const int index = indexOf(childTree);
        (void)index;
        jassert(index >= 0);

        if (ObjectType* newObject = createNewObject(childTree))
        {
            if (index == getNumChildren() - 1)
                _objects.add(newObject);
            else
                _objects.addSorted(*this, newObject);

            onObjectAdded(*newObject);
        }
        else
            jassertfalse;
    }

    /**
     * @brief Will execute the callback `onObjectAdded` if it exists.
     *
     * @param parentTree
     * @param childTree
     * @param index
     */
    virtual void valueTreeChildRemoved (juce::ValueTree& parentTree, juce::ValueTree& childTree,
                                int index) override
    {
        Object::valueTreeChildRemoved(parentTree, childTree, index);

        if (parentTree != *this)
            return;

        if (ObjectType* o = getObject(index))
        {
            _objects.removeObject(o, false);
            onObjectRemoved(*o);
            delete o;
        }
    }

    /**
     * @brief will execute the callback `onObjectOrderChanged` if it exists.
     *
     * @param parentTree
     * @param oldIndex
     * @param newIndex
     */
    virtual void valueTreeChildOrderChanged (juce::ValueTree& childTree, int oldIndex, int newIndex) override
    {
        Object::valueTreeChildOrderChanged(childTree, oldIndex, newIndex);

        if (childTree.getParent() != *this)
            return;

        _objects.sort(*this);
        onObjectOrderChanged(oldIndex, newIndex);
    }

public:
    /**
     * Compares two elements of the object list.
     * This function is needed for sorting and sorted insertions on the list
     * 
     * @param first The first element to compare.
     * @param second The second element to compare.
     * @return A negative value if the first element comes before the second element,
     *         a positive value if the first element comes after the second element,
     *         or zero if the elements are equal.
     */
    int compareElements(ObjectType* first, const ObjectType* second) const
    {
        int index1 = indexOf(*first);
        int index2 = indexOf(*second);
        return index1 - index2;
    }

private:
    juce::OwnedArray<ObjectType> _objects;
};

} // namespace cello
