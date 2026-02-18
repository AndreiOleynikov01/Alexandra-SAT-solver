#include "Alexandra.h"

namespace Utilities
{
	Stack::Entry::Entry(int value, Entry* prev) : value(value), prev(prev) {}

	Stack::Entry::Entry(int value) : value(value), prev(NULL) {}

	Stack::Entry::Entry(Entry* entry) : value(entry->value), prev(entry->prev) {}

	Stack::Stack() : entry(new Entry(0)) {}

	Stack::Stack(Stack* stack) : entry(stack->top_entry()) {}

	Stack& Stack::operator=(Stack& stack)
	{
		return new Stack(stack);
	}

	int Stack::top()
	{
		return entry->value;
	}

	Stack::Entry* Stack::top_entry()
	{
		return entry;
	}

	void Stack::push(int value)
	{
		Stack::Entry* new_entry = new Entry(value, entry);
		entry = new_entry;
	}

	Stack::Entry* Stack::pop()
	{
		Stack::Entry* new_entry = entry;
		entry = entry->prev;
		return entry;
	}
}