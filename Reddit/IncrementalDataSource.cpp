#include "pch.h"
#include "IncrementalDataSource.h"


Reddit::IncrementalDataSource::IncrementalDataSource(Platform::Collections::Vector<Platform::Object^>% v) : vec(v)
{

}
Windows::UI::Xaml::Interop::IBindableIterator ^ Reddit::IncrementalDataSource::First()
{
	return dynamic_cast<Windows::UI::Xaml::Interop::IBindableIterator ^>(vec.First());
}

Platform::Object ^ Reddit::IncrementalDataSource::GetAt(unsigned int index)
{
	return vec.GetAt(std::move(index));
}

Windows::UI::Xaml::Interop::IBindableVectorView ^ Reddit::IncrementalDataSource::GetView()
{
	return safe_cast<Windows::UI::Xaml::Interop::IBindableVectorView ^ >(vec.GetView());
}

bool Reddit::IncrementalDataSource::IndexOf(Platform::Object ^value, unsigned int *index)
{
	return vec.IndexOf(std::move(value), index);
}

void Reddit::IncrementalDataSource::SetAt(unsigned int index, Platform::Object ^value)
{
	vec.SetAt(std::move(index), std::move(value));
}

void Reddit::IncrementalDataSource::InsertAt(unsigned int index, Platform::Object ^value)
{
	vec.InsertAt(index, std::move(value));
}

void Reddit::IncrementalDataSource::RemoveAt(unsigned int index)
{
	vec.RemoveAt(index);
}

void Reddit::IncrementalDataSource::Append(Platform::Object ^value)
{
	vec.Append(std::move(value));
}

void Reddit::IncrementalDataSource::RemoveAtEnd()
{
	vec.RemoveAtEnd();
}

void Reddit::IncrementalDataSource::Clear()
{
	vec.Clear();
}

Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Data::LoadMoreItemsResult> ^ Reddit::IncrementalDataSource::LoadMoreItemsAsync(unsigned int count)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}
